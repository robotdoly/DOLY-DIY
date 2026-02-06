#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include "TouchControl.h"
#include "TouchEvent.h"
#include "TouchEventListener.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_touch;
static py::function g_on_touch_activity;

static bool g_touch_registered = false;
static bool g_touch_activity_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// TouchEvent stores raw TouchEventListener* pointers. If Python garbage-collects the
// listener object while still registered, C++ can call a dangling pointer and crash.
// To prevent that, we keep strong references to Python listener objects until they
// are explicitly removed or cleared.
static std::mutex g_listener_mutex;
static std::vector<py::object> g_py_listeners;

static void safe_call(py::function &cb, auto&&... args)
{
    if (!cb) return;
    try {
        cb(std::forward<decltype(args)>(args)...);
    } catch (const py::error_already_set&) {
        // Don't let Python exceptions unwind into C++
        PyErr_Print();
    }
}

// Trampolines matching static TouchEvent signatures
static void on_touch_trampoline(TouchSide side, TouchState state)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_touch; }

    safe_call(cb, side, state);
}

static void on_touch_activity_trampoline(TouchSide side, TouchActivity activity)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_touch_activity; }

    safe_call(cb, side, activity);
}

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyTouchEventListener : public TouchEventListener {
public:
    using TouchEventListener::TouchEventListener;

    void onTouchEvent(TouchSide side, TouchState state) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, TouchEventListener, onTouchEvent, side, state);
    }

    void onTouchActivityEvent(TouchSide side, TouchActivity activity) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, TouchEventListener, onTouchActivityEvent, side, activity);
    }
};

// Internal helper used by clear_listeners() and dispose()
static void clear_listeners_impl()
{
    // Disable first so any in-flight C++ events will become no-ops
    g_callbacks_enabled.store(false, std::memory_order_relaxed);

    // 1) Clear static callback trampolines + stored Python functions
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (g_touch_registered) {
            TouchEvent::RemoveListenerOnTouch(on_touch_trampoline);
            g_touch_registered = false;
        }
        if (g_touch_activity_registered) {
            TouchEvent::RemoveListenerOnTouchActivity(on_touch_activity_trampoline);
            g_touch_activity_registered = false;
        }

        g_on_touch = py::function();
        g_on_touch_activity = py::function();
    }

    // 2) Unregister any class-based listeners we kept alive, then drop refs.
    // (This function is called from Python land, so the GIL is held here.)
    std::vector<py::object> listeners;
    {
        std::lock_guard<std::mutex> lock(g_listener_mutex);
        listeners.swap(g_py_listeners);
    }
    for (auto &obj : listeners) {
        try {
            auto *ptr = obj.cast<TouchEventListener*>();
            if (ptr) TouchEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

// -------------------------------
// Module
// -------------------------------
PYBIND11_MODULE(doly_touch, m) {
    m.doc() = "Bindings for TouchControl / TouchEvent (pybind11)";

    py::enum_<TouchSide>(m, "TouchSide")
        .value("Both", TouchSide::BOTH)
        .value("Left", TouchSide::LEFT)
        .value("Right", TouchSide::RIGHT);

    py::enum_<TouchState>(m, "TouchState")
        .value("Up", TouchState::UP)
        .value("Down", TouchState::DOWN);

    py::enum_<TouchActivity>(m, "TouchActivity")
        .value("Patting", TouchActivity::PATTING)
        .value("Disturb", TouchActivity::DISTURB);

    // Listener base class (object-based)
    py::class_<TouchEventListener, PyTouchEventListener>(m, "TouchEventListener")
        .def(py::init<>())
        .def("onTouchEvent", &TouchEventListener::onTouchEvent)
        .def("onTouchActivityEvent", &TouchEventListener::onTouchActivityEvent);

    m.def(
        "add_listener",
        [](py::object listener_obj, bool priority) {
            // Register with the C++ event system
            auto *listener = listener_obj.cast<TouchEventListener*>();
            TouchEvent::AddListener(listener, priority);

            // Keep the Python object alive to prevent GC -> dangling pointer
            std::lock_guard<std::mutex> lock(g_listener_mutex);
            // Avoid storing duplicates if the same object is added twice.
            for (const auto &o : g_py_listeners) {
                if (o.is(listener_obj)) {
                    return;
                }
            }
            g_py_listeners.emplace_back(std::move(listener_obj));
        },
        py::arg("listener"), py::arg("priority") = false,
        "Register a class-based listener (kept alive by the binding until removed/cleared)."
    );

    m.def(
        "remove_listener",
        [](py::object listener_obj) {
            auto *listener = listener_obj.cast<TouchEventListener*>();
            TouchEvent::RemoveListener(listener);

            // Drop the strong reference if we hold it
            std::lock_guard<std::mutex> lock(g_listener_mutex);
            g_py_listeners.erase(
                std::remove_if(g_py_listeners.begin(), g_py_listeners.end(),
                               [&](const py::object &o) { return o.is(listener_obj); }),
                g_py_listeners.end()
            );
        },
        py::arg("listener"),
        "Unregister a class-based listener."
    );

    // -------------------------------
    // Static events -> single Python callback per event type
    // -------------------------------
    m.def(
        "on_touch",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            // Replace callback
            g_on_touch = std::move(cb);

            // Ensure trampoline is registered exactly once
            if (!g_touch_registered) {
                TouchEvent::AddListenerOnTouch(on_touch_trampoline);
                g_touch_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static touch callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_touch_activity",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_touch_activity = std::move(cb);

            if (!g_touch_activity_registered) {
                TouchEvent::AddListenerOnTouchActivity(on_touch_activity_trampoline);
                g_touch_activity_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static touch-activity callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "clear_listeners",
        []() { clear_listeners_impl(); },
        "Unregister all static callbacks and clear stored Python functions."
    );

    // -------------------------------
    // TouchControl API
    // -------------------------------
    m.def(
        "init",
        []() {
            g_callbacks_enabled.store(true, std::memory_order_relaxed);
            return TouchControl::init();
        },
        "Initialize touch control."
    );

    // Safer dispose: clear callbacks first
    m.def(
        "dispose",
        []() {
            clear_listeners_impl();
            return TouchControl::dispose();
        },
        "Dispose touch control (also clears static callbacks first)."
    );

    m.def("is_active", &TouchControl::isActive, "Return True if initialized/active.");
    m.def("is_touched", &TouchControl::isTouched, py::arg("side"), "Return True if side is touched.");
    m.def("get_version", &TouchControl::getVersion, "Get version (0.XYZ).");
}
