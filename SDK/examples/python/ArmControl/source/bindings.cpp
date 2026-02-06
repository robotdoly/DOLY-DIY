#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <mutex>
#include <atomic>
#include <vector>

#include "ArmControl.h"
#include "ArmEvent.h"
#include "ArmEventListener.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_complete;
static py::function g_on_error;
static py::function g_on_state_change;
static py::function g_on_movement;

static bool g_complete_registered = false;
static bool g_error_registered = false;
static bool g_state_registered = false;
static bool g_movement_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// ArmEvent stores raw ArmEventListener* pointers. If Python garbage-collects the
// listener object while still registered, C++ can call a dangling pointer and
// crash. To prevent that, we keep strong references to Python listener objects
// until they are explicitly removed or cleared.
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

// Trampolines matching static ArmEvent signatures
static void on_complete_trampoline(std::uint16_t id, ArmSide side)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_complete; }

    safe_call(cb, id, side);
}

static void on_error_trampoline(std::uint16_t id, ArmSide side, ArmErrorType type)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_error; }

    safe_call(cb, id, side, type);
}

static void on_state_change_trampoline(ArmSide side, ArmState state)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_state_change; }

    safe_call(cb, side, state);
}

static void on_movement_trampoline(ArmSide side, float change)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_movement; }

    safe_call(cb, side, change);
}

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyArmEventListener : public ArmEventListener {
public:
    using ArmEventListener::ArmEventListener;

    void onArmComplete(uint16_t id, ArmSide side) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ArmEventListener, onArmComplete, id, side);
    }

    void onArmError(uint16_t id, ArmSide side, ArmErrorType errorType) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ArmEventListener, onArmError, id, side, errorType);
    }

    void onArmStateChange(ArmSide side, ArmState state) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ArmEventListener, onArmStateChange, side, state);
    }

    void onArmMovement(ArmSide side, float degreeChange) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ArmEventListener, onArmMovement, side, degreeChange);
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
            ArmEvent::RemoveListenerOnComplete(on_complete_trampoline);
            g_complete_registered = false;
        }
        if (g_error_registered) {
            ArmEvent::RemoveListenerOnError(on_error_trampoline);
            g_error_registered = false;
        }
        if (g_state_registered) {
            ArmEvent::RemoveListenerOnStateChange(on_state_change_trampoline);
            g_state_registered = false;
        }
        if (g_movement_registered) {
            ArmEvent::RemoveListenerOnMovement(on_movement_trampoline);
            g_movement_registered = false;
        }

        g_on_complete = py::function();
        g_on_error = py::function();
        g_on_state_change = py::function();
        g_on_movement = py::function();
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
            auto *ptr = obj.cast<ArmEventListener*>();
            if (ptr) ArmEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

// -------------------------------
// Module
// -------------------------------
PYBIND11_MODULE(doly_arm, m) {
    m.doc() = "Bindings for ArmControl / ArmEvent (pybind11)";

    py::enum_<ArmErrorType>(m, "ArmErrorType")
        .value("Abort", ArmErrorType::ABORT)
        .value("Motor", ArmErrorType::MOTOR);

    py::enum_<ArmSide>(m, "ArmSide")
        .value("Both", ArmSide::BOTH)
        .value("Left", ArmSide::LEFT)
        .value("Right", ArmSide::RIGHT);

    py::enum_<ArmState>(m, "ArmState")
        .value("Running", ArmState::RUNNING)
        .value("Completed", ArmState::COMPLETED)
        .value("Error", ArmState::ERROR);

    py::class_<ArmData>(m, "ArmData")
        .def(py::init<>())
        .def_readwrite("side", &ArmData::side)
        .def_readwrite("angle", &ArmData::angle);

    // Listener base class (object-based)
    py::class_<ArmEventListener, PyArmEventListener>(m, "ArmEventListener")
        .def(py::init<>())
        .def("onArmComplete", &ArmEventListener::onArmComplete)
        .def("onArmError", &ArmEventListener::onArmError)
        .def("onArmStateChange", &ArmEventListener::onArmStateChange)
        .def("onArmMovement", &ArmEventListener::onArmMovement);

    m.def("add_listener",
          [](py::object listener_obj, bool priority) {
              // Register with the C++ event system
              auto *listener = listener_obj.cast<ArmEventListener*>();
              ArmEvent::AddListener(listener, priority);

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
          "Register a class-based listener (kept alive by the binding until removed/cleared).");

    m.def("remove_listener",
          [](py::object listener_obj) {
              auto *listener = listener_obj.cast<ArmEventListener*>();
              ArmEvent::RemoveListener(listener);

              // Drop the strong reference if we hold it
              std::lock_guard<std::mutex> lock(g_listener_mutex);
              g_py_listeners.erase(
                  std::remove_if(g_py_listeners.begin(), g_py_listeners.end(),
                                 [&](const py::object &o) { return o.is(listener_obj); }),
                  g_py_listeners.end());
          },
          py::arg("listener"),
          "Unregister a class-based listener.");

    // -------------------------------
    // Static events -> single Python callback per event type
    // -------------------------------
    m.def("on_complete", [](py::function cb) {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        g_callbacks_enabled.store(true, std::memory_order_relaxed);

        // Replace callback
        g_on_complete = std::move(cb);

        // Ensure trampoline is registered exactly once
        if (!g_complete_registered) {
            ArmEvent::AddListenerOnComplete(on_complete_trampoline);
            g_complete_registered = true;
        }
    }, py::arg("cb"),
    "Set a static complete callback (replaces any previous one).\n"
    "Tip: If you want multiple handlers, use a Python dispatcher function.");

    m.def("on_error", [](py::function cb) {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        g_callbacks_enabled.store(true, std::memory_order_relaxed);

        g_on_error = std::move(cb);

        if (!g_error_registered) {
            ArmEvent::AddListenerOnError(on_error_trampoline);
            g_error_registered = true;
        }
    });

    m.def("on_state_change", [](py::function cb) {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        g_callbacks_enabled.store(true, std::memory_order_relaxed);

        g_on_state_change = std::move(cb);

        if (!g_state_registered) {
            ArmEvent::AddListenerOnStateChange(on_state_change_trampoline);
            g_state_registered = true;
        }
    });

    m.def("on_movement", [](py::function cb) {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        g_callbacks_enabled.store(true, std::memory_order_relaxed);

        g_on_movement = std::move(cb);

        if (!g_movement_registered) {
            ArmEvent::AddListenerOnMovement(on_movement_trampoline);
            g_movement_registered = true;
        }
    });

    m.def("clear_listeners", []() {
        clear_listeners_impl();
    }, "Unregister all static callbacks and clear stored Python functions.");

    // -------------------------------
    // ArmControl API
    // -------------------------------
    m.def("init", []() {
        g_callbacks_enabled.store(true, std::memory_order_relaxed);
        return ArmControl::init();
    }, "Initialize arm control.");

    // Safer dispose: clear callbacks first
    m.def("dispose", []() {
        clear_listeners_impl();
        return ArmControl::dispose();
    }, "Dispose arm control (also clears static callbacks first).");

    m.def("is_active", &ArmControl::isActive, "Return True if initialized/active.");
    m.def("abort", &ArmControl::Abort, py::arg("side"), "Abort operations for side.");
    m.def("get_max_angle", &ArmControl::getMaxAngle, "Get maximum angle limit.");

    m.def("set_angle", &ArmControl::setAngle,
          py::arg("id"), py::arg("side"), py::arg("speed"), py::arg("angle"),
          py::arg("with_brake") = false,
          "Set arm angle.\n"
          "Returns:\n"
          " 0 success\n"
          "-1 not active\n"
          "-2 speed range error\n"
          "-3 angle range error");

    m.def("get_state", &ArmControl::getState, py::arg("side"), "Get ArmState.");
    m.def("get_current_angle", &ArmControl::getCurrentAngle, py::arg("side"),
          "Get current angles. Returns list[ArmData].");
    m.def("get_version", &ArmControl::getVersion, "Get version (0.XYZ).");
    
}
