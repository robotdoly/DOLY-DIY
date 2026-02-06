#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include "ServoControl.h"
#include "ServoEvent.h"
#include "ServoEventListener.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_complete;
static py::function g_on_abort;
static py::function g_on_error;

static bool g_complete_registered = false;
static bool g_abort_registered = false;
static bool g_error_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// ServoEvent stores raw ServoEventListener* pointers. If Python garbage-collects the
// listener object while still registered, C++ can call a dangling pointer and crash.
// To prevent that, we keep strong references to Python listener objects until they
// are explicitly removed or cleared.
static std::mutex g_listener_mutex;
static std::vector<py::object> g_py_listeners;

template <class... Args>
static void safe_call(py::function &cb, Args&&... args)
{
    if (!cb) return;
    try {
        cb(std::forward<Args>(args)...);
    } catch (const py::error_already_set&) {
        // Don't let Python exceptions unwind into C++
        PyErr_Print();
    }
}

// Trampolines matching static ServoEvent signatures
static void on_complete_trampoline(uint16_t id, ServoId channel)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_complete; }

    safe_call(cb, id, channel);
}

static void on_abort_trampoline(uint16_t id, ServoId channel)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_abort; }

    safe_call(cb, id, channel);
}

static void on_error_trampoline(uint16_t id, ServoId channel)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_error; }

    safe_call(cb, id, channel);
}

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyServoEventListener : public ServoEventListener {
public:
    using ServoEventListener::ServoEventListener;

    void onServoAbort(uint16_t id, ServoId channel) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ServoEventListener, onServoAbort, id, channel);
    }

    void onServoError(uint16_t id, ServoId channel) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ServoEventListener, onServoError, id, channel);
    }

    void onServoComplete(uint16_t id, ServoId channel) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ServoEventListener, onServoComplete, id, channel);
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

        if (g_complete_registered) {
            ServoEvent::RemoveListenerOnComplete(on_complete_trampoline);
            g_complete_registered = false;
        }
        if (g_abort_registered) {
            ServoEvent::RemoveListenerOnAbort(on_abort_trampoline);
            g_abort_registered = false;
        }
        if (g_error_registered) {
            ServoEvent::RemoveListenerOnError(on_error_trampoline);
            g_error_registered = false;
        }

        g_on_complete = py::function();
        g_on_abort = py::function();
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
            auto *ptr = obj.cast<ServoEventListener*>();
            if (ptr) ServoEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

// -------------------------------
// Module
// -------------------------------
PYBIND11_MODULE(doly_servo, m) {
    m.doc() = "Bindings for ServoControl / ServoEvent (pybind11)";

    py::enum_<ServoId>(m, "ServoId")
        .value("Servo0", ServoId::SERVO_0)
        .value("Servo1", ServoId::SERVO_1);

    // Listener base class (object-based)
    py::class_<ServoEventListener, PyServoEventListener>(m, "ServoEventListener")
        .def(py::init<>())
        .def("onServoAbort", &ServoEventListener::onServoAbort)
        .def("onServoError", &ServoEventListener::onServoError)
        .def("onServoComplete", &ServoEventListener::onServoComplete);

    m.def(
        "add_listener",
        [](py::object listener_obj, bool priority) {
            auto *listener = listener_obj.cast<ServoEventListener*>();
            ServoEvent::AddListener(listener, priority);

            // Keep the Python object alive to prevent GC -> dangling pointer
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
            auto *listener = listener_obj.cast<ServoEventListener*>();
            ServoEvent::RemoveListener(listener);

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
        "on_complete",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_complete = std::move(cb);
            if (!g_complete_registered) {
                ServoEvent::AddListenerOnComplete(on_complete_trampoline);
                g_complete_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static servo complete callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_abort",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_abort = std::move(cb);
            if (!g_abort_registered) {
                ServoEvent::AddListenerOnAbort(on_abort_trampoline);
                g_abort_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static servo abort callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_error",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_error = std::move(cb);
            if (!g_error_registered) {
                ServoEvent::AddListenerOnError(on_error_trampoline);
                g_error_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static servo error callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "clear_listeners",
        []() { clear_listeners_impl(); },
        "Unregister all static callbacks and clear stored Python functions."
    );

    // -------------------------------
    // ServoControl API
    // -------------------------------
    m.def(
        "init",
        []() {
            g_callbacks_enabled.store(true, std::memory_order_relaxed);
            return ServoControl::init();
        },
        "Initialize servo control."
    );

    m.def(
        "set_servo",
        [](uint16_t id, ServoId channel, float angle, uint8_t speed, bool invert) {
            return ServoControl::setServo(id, channel, angle, speed, invert);
        },
        py::arg("id"), py::arg("channel"), py::arg("angle"), py::arg("speed") = 100, py::arg("invert") = false,
        "Set servo angle.\n"
        "Returns 0 on success or a negative error code."
    );

    m.def("abort", &ServoControl::abort, py::arg("channel"),
          "Abort ongoing servo process.");

    m.def("release", &ServoControl::release, py::arg("channel"),
          "Release servo hold.\n"
          "Returns 0 on success or a negative error code.");

    m.def(
        "dispose",
        []() {
            clear_listeners_impl();
            return ServoControl::dispose();
        },
        "Dispose servo control (also clears callbacks/listeners first)."
    );

    m.def("get_version", &ServoControl::getVersion, "Get version (0.XYZ).");
}
