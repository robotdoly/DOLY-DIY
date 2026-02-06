#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include "LedControl.h"
#include "LedEvent.h"
#include "LedEventListener.h"
#include "Color.h"  // LedActivity depends on Color (registered by doly_color)

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_complete;
static py::function g_on_error;

static bool g_complete_registered = false;
static bool g_error_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
static std::mutex g_listener_mutex;
static std::vector<py::object> g_py_listeners;

template <typename... Args>
static void safe_call(py::function &cb, Args &&...args) {
    if (!cb) return;
    try {
        cb(std::forward<Args>(args)...);
    } catch (const py::error_already_set &) {
        // Don't let Python exceptions unwind into C++
        PyErr_Print();
    }
}

// Trampolines matching static LedEvent signatures
static void on_complete_trampoline(uint16_t id, LedSide side) {
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;
    py::function cb;
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        cb = g_on_complete;
    }
    safe_call(cb, id, side);
}

static void on_error_trampoline(uint16_t id, LedSide side, LedErrorType type) {
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;
    py::function cb;
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        cb = g_on_error;
    }
    safe_call(cb, id, side, type);
}


// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyLedEventListener : public LedEventListener {
public:
    using LedEventListener::LedEventListener;

    void onLedComplete(uint16_t id, LedSide side) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, LedEventListener, onLedComplete, id, side);
    }

    void onLedError(uint16_t id, LedSide side, LedErrorType type) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, LedEventListener, onLedError, id, side, type);
    }

};

// Internal helper used by clear_listeners() and dispose()
static void clear_listeners_impl() {
    // Disable first so any in-flight C++ events will become no-ops
    g_callbacks_enabled.store(false, std::memory_order_relaxed);

    // 1) Clear static callback trampolines + stored Python functions
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (g_complete_registered) {
            LedEvent::RemoveListenerOnComplete(on_complete_trampoline);
            g_complete_registered = false;
        }
        if (g_error_registered) {
            LedEvent::RemoveListenerOnError(on_error_trampoline);
            g_error_registered = false;
        }

        g_on_complete = py::function();
        g_on_error = py::function();

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
            auto *ptr = obj.cast<LedEventListener *>();
            if (ptr) LedEvent::RemoveListener(ptr);
        } catch (const py::cast_error &) {
            // ignore
        }
    }
}

// -------------------------------
// Module
// -------------------------------
PYBIND11_MODULE(doly_led, m) {
    m.doc() = "Bindings for LedControl / LedEvent (pybind11)";

    // Ensure Color / ColorCode are registered exactly once (shared across modules)
    py::module_::import("doly_color");

    py::enum_<LedSide>(m, "LedSide")
        .value("Both", LedSide::BOTH)
        .value("Left", LedSide::LEFT)
        .value("Right", LedSide::RIGHT);

    py::enum_<LedActivityState>(m, "LedActivityState")
        .value("Free", LedActivityState::FREE)
        .value("Running", LedActivityState::RUNNING)
        .value("Completed", LedActivityState::COMPLETED);

    py::enum_<LedErrorType>(m, "LedErrorType").value("Abort", LedErrorType::ABORT);

    py::class_<LedActivity>(m, "LedActivity")
        .def(py::init<>())
        .def_readwrite("mainColor", &LedActivity::mainColor)
        .def_readwrite("fadeColor", &LedActivity::fadeColor)
        .def_readwrite("fade_time", &LedActivity::fade_time)
        .def_readwrite("state", &LedActivity::state);

    // Listener base class (object-based)
    py::class_<LedEventListener, PyLedEventListener>(m, "LedEventListener")
        .def(py::init<>())
        .def("onLedComplete", &LedEventListener::onLedComplete)
        .def("onLedError", &LedEventListener::onLedError);

    m.def(
        "add_listener",
        [](py::object listener_obj, bool priority) {
            auto *listener = listener_obj.cast<LedEventListener *>();
            LedEvent::AddListener(listener, priority);

            std::lock_guard<std::mutex> lock(g_listener_mutex);
            for (const auto &o : g_py_listeners) {
                if (o.is(listener_obj)) return;
            }
            g_py_listeners.emplace_back(std::move(listener_obj));
        },
        py::arg("listener"), py::arg("priority") = false,
        "Register a class-based listener (kept alive by the binding until removed/cleared)."
    );

    m.def(
        "remove_listener",
        [](py::object listener_obj) {
            auto *listener = listener_obj.cast<LedEventListener *>();
            LedEvent::RemoveListener(listener);

            std::lock_guard<std::mutex> lock(g_listener_mutex);
            g_py_listeners.erase(
                std::remove_if(g_py_listeners.begin(), g_py_listeners.end(),
                               [&](const py::object &o) { return o.is(listener_obj); }),
                g_py_listeners.end());
        },
        py::arg("listener"),
        "Unregister a class-based listener."
    );

    // -------------------------------
    // Static events -> single Python callback per event type
    // -------------------------------
    m.def(
        "on_complete",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_complete = std::move(cb);
            if (!g_complete_registered) {
                LedEvent::AddListenerOnComplete(on_complete_trampoline);
                g_complete_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static complete callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_error",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_error = std::move(cb);
            if (!g_error_registered) {
                LedEvent::AddListenerOnError(on_error_trampoline);
                g_error_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static error callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );      

    m.def(
        "clear_listeners",
        []() { clear_listeners_impl(); },
        "Unregister all static callbacks and clear stored Python functions."
    );

    // -------------------------------
    // LedControl API
    // -------------------------------
    m.def(
        "init",
        []() {
            g_callbacks_enabled.store(true, std::memory_order_relaxed);
            return LedControl::init();
        },
        "Initialize LED control."
    );

    // Safer dispose: clear callbacks first
    m.def(
        "dispose",
        []() {
            clear_listeners_impl();
            return LedControl::dispose();
        },
        "Dispose LED control (also clears callbacks first)."
    );

    m.def("is_active", &LedControl::isActive, "Return True if initialized/active.");
    m.def("abort", &LedControl::Abort, py::arg("side"), "Abort LED processing for side.");
    m.def(
        "process_activity",
        &LedControl::processActivity,
        py::arg("id"), py::arg("side"), py::arg("activity"),
        "Process a LED activity (fade, timing)."
    );
    m.def("get_version", &LedControl::getVersion, "Get version (0.XYZ)." );
}
