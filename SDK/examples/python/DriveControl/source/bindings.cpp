#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <atomic>
#include <algorithm>
#include <mutex>
#include <vector>

#include "DriveControl.h"
#include "DriveEvent.h"
#include "DriveEventListener.h"

namespace py = pybind11;

// ------------------------------
// Lifetime / thread-safety guards
// ------------------------------

// Gate to prevent late callbacks during shutdown / interpreter teardown.
static std::atomic<bool> g_callbacks_enabled{true};

// Store Python callbacks + registration flags.
// Note: We keep these as strong refs so callbacks remain alive.
static std::mutex g_cb_mutex;
static py::function g_on_complete;
static py::function g_on_error;
static py::function g_on_state_change;
static bool g_complete_registered = false;
static bool g_error_registered = false;
static bool g_state_registered = false;

// ------------------------------
// Lifetime safety for class-based listeners
// ------------------------------
// DriveEvent stores raw DriveEventListener* pointers. If Python garbage-collects
// a listener object while still registered, C++ can call a dangling pointer and
// crash. We keep strong references to Python listener objects until they are
// explicitly removed or cleared.
static std::mutex g_listener_mutex;
static std::vector<py::object> g_py_listeners;

// Call Python safely: never let Python exceptions unwind into C++.
static inline void safe_call(const py::function &fn, auto&&... args) {
    if (!fn) return;
    try {
        fn(std::forward<decltype(args)>(args)...);
    } catch (const py::error_already_set&) {
        PyErr_Print();
    }
}

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyDriveEventListener : public DriveEventListener {
public:
    using DriveEventListener::DriveEventListener;

    void onDriveComplete(uint16_t id) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, DriveEventListener, onDriveComplete, id);
    }

    void onDriveError(uint16_t id, DriveMotorSide side, DriveErrorType type) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, DriveEventListener, onDriveError, id, side, type);
    }

    void onDriveStateChange(DriveType driveType, DriveState state) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, DriveEventListener, onDriveStateChange, driveType, state);
    }
};

// Trampolines matching DriveEvent signatures.
// IMPORTANT: These may be called from non-Python threads, so we must:
// 1) check gate, 2) acquire GIL, 3) copy callback under mutex, 4) call without holding mutex.
static void on_complete_trampoline(std::uint16_t id)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;
    py::function cb;
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        cb = g_on_complete; // copy strong ref
    }
    safe_call(cb, id);
}

static void on_error_trampoline(std::uint16_t id, DriveMotorSide side, DriveErrorType type)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;
    py::function cb;
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        cb = g_on_error;
    }
    safe_call(cb, id, side, type);
}

static void on_state_change_trampoline(DriveType drive_type, DriveState state)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;
    py::function cb;
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        cb = g_on_state_change;
    }
    safe_call(cb, drive_type, state);
}

// Internal: unregister listeners + drop callback refs.
// Called from Python land (GIL held), but safe to call regardless.
static void clear_listeners_impl()
{   
    // Disable first so any in-flight C++ events will become no-ops
    g_callbacks_enabled.store(false, std::memory_order_relaxed);    

    // 1) Unregister static callbacks + drop stored Python functions
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (g_complete_registered) {
            DriveEvent::RemoveListenerOnComplete(on_complete_trampoline);
            g_complete_registered = false;
        }
        if (g_error_registered) {
            DriveEvent::RemoveListenerOnError(on_error_trampoline);
            g_error_registered = false;
        }
        if (g_state_registered) {
            DriveEvent::RemoveListenerOnStateChange(on_state_change_trampoline);
            g_state_registered = false;
        }

        g_on_complete = py::function();
        g_on_error = py::function();
        g_on_state_change = py::function();
    }

    // 2) Unregister any class-based listeners we kept alive, then drop refs.
    // (Called from Python -> the GIL is held here.)
    std::vector<py::object> listeners;
    {
        std::lock_guard<std::mutex> lk(g_listener_mutex);
        listeners.swap(g_py_listeners);
    }
    for (auto &obj : listeners) {
        try {
            auto *ptr = obj.cast<DriveEventListener*>();
            if (ptr) DriveEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }   
}

PYBIND11_MODULE(doly_drive, m)
{
    m.doc() = "Python wrapper for Doly DriveControl (static C++ library)";

    // Enums
    py::enum_<DriveErrorType>(m, "DriveErrorType")
        .value("Abort", DriveErrorType::ABORT)
        .value("Force", DriveErrorType::FORCE)
        .value("Rotate", DriveErrorType::ROTATE)
        .value("Motor", DriveErrorType::MOTOR)
        .export_values();

    py::enum_<DriveMotorSide>(m, "DriveMotorSide")
        .value("Both", DriveMotorSide::BOTH)
        .value("Left", DriveMotorSide::LEFT)
        .value("Right", DriveMotorSide::RIGHT)
        .export_values();

    py::enum_<DriveState>(m, "DriveState")
        .value("Running", DriveState::RUNNING)
        .value("Completed", DriveState::COMPLETED)
        .value("Error", DriveState::ERROR)
        .export_values();

    py::enum_<DriveType>(m, "DriveType")
        .value("Freestyle", DriveType::FREESTYLE)
        .value("XY", DriveType::XY)
        .value("Distance", DriveType::DISTANCE)
        .value("Rotate", DriveType::ROTATE)
        .export_values();

    // ------------------------------
    // Class-based event listener API
    // ------------------------------
    py::class_<DriveEventListener, PyDriveEventListener>(m, "DriveEventListener")
        .def(py::init<>())
        .def("on_drive_complete", &DriveEventListener::onDriveComplete)
        .def("on_drive_error", &DriveEventListener::onDriveError)
        .def("on_drive_state_change", &DriveEventListener::onDriveStateChange);

    // Register a listener instance (keepalive to prevent GC while registered)
    m.def("add_listener", [](py::object listener_obj) {
        if (listener_obj.is_none()) return;

        auto *ptr = listener_obj.cast<DriveEventListener*>();
        if (!ptr) return;

        // Avoid double-keeping the same listener
        {
            std::lock_guard<std::mutex> lk(g_listener_mutex);
            for (auto &o : g_py_listeners) {
                try {
                    if (o.cast<DriveEventListener*>() == ptr) {
                        // Already registered/kept alive
                        return;
                    }
                } catch (const py::cast_error&) {
                    // ignore
                }
            }
            g_py_listeners.push_back(listener_obj);
        }

        DriveEvent::AddListener(ptr);
    });

    // Unregister a listener instance (also drop keepalive ref)
    m.def("remove_listener", [](py::object listener_obj) {
        if (listener_obj.is_none()) return;

        auto *ptr = listener_obj.cast<DriveEventListener*>();
        if (!ptr) return;

        DriveEvent::RemoveListener(ptr);

        std::lock_guard<std::mutex> lk(g_listener_mutex);
        g_py_listeners.erase(
            std::remove_if(g_py_listeners.begin(), g_py_listeners.end(),
                           [&](py::object &o) {
                               try { return o.cast<DriveEventListener*>() == ptr; }
                               catch (const py::cast_error&) { return false; }
                           }),
            g_py_listeners.end());
    });

    // DriveControl functions
    m.def("init", &DriveControl::init,
          py::arg("imu_off_gx") = 0, py::arg("imu_off_gy") = 0, py::arg("imu_off_gz") = 0,
          py::arg("imu_off_ax") = 0, py::arg("imu_off_ay") = 0, py::arg("imu_off_az") = 0);

    // Dispose wrapper: disables callbacks, unregisters listeners, then disposes the library.
    m.def("dispose", [](bool dispose_IMU) {
        // Called from Python -> GIL held.
        clear_listeners_impl();
        DriveControl::dispose(dispose_IMU);
    }, py::arg("dispose_IMU"));

    m.def("is_active", &DriveControl::isActive);
    m.def("abort", &DriveControl::Abort);

    m.def("free_drive", &DriveControl::freeDrive,
          py::arg("speed"), py::arg("is_left"), py::arg("to_forward"));

    m.def("go_xy", &DriveControl::goXY,
          py::arg("id"), py::arg("x"), py::arg("y"), py::arg("speed"), py::arg("to_forward"),
          py::arg("with_brake") = false, py::arg("acceleration_interval") = 0,
          py::arg("control_speed") = false, py::arg("control_force") = true);

    m.def("go_distance", &DriveControl::goDistance,
          py::arg("id"), py::arg("mm"), py::arg("speed"), py::arg("to_forward"),
          py::arg("with_brake") = false, py::arg("acceleration_interval") = 0,
          py::arg("control_speed") = false, py::arg("control_force") = true);

    m.def("go_rotate", &DriveControl::goRotate,
          py::arg("id"), py::arg("rotate_angle"), py::arg("from_center"),
          py::arg("speed"), py::arg("to_forward"),
          py::arg("with_brake") = false, py::arg("acceleration_interval") = 0,
          py::arg("control_speed") = false, py::arg("control_force") = true);

    m.def("get_position", &DriveControl::getPosition);
    m.def("reset_position", &DriveControl::resetPosition);
    m.def("get_state", &DriveControl::getState);
    m.def("get_rpm", &DriveControl::getRPM, py::arg("is_left"));

    // ------------------------------
    // Events -> Python callbacks
    // ------------------------------
    // Accept None to unregister.
    m.def("on_complete", [](py::object cb_obj) {
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (cb_obj.is_none()) {
            if (g_complete_registered) {
                DriveEvent::RemoveListenerOnComplete(on_complete_trampoline);
                g_complete_registered = false;
            }
            g_on_complete = py::function();
            return;
        }

        g_on_complete = cb_obj.cast<py::function>();
        if (!g_complete_registered) {
            DriveEvent::AddListenerOnComplete(on_complete_trampoline);
            g_complete_registered = true;
        }
    });

    m.def("on_error", [](py::object cb_obj) {
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (cb_obj.is_none()) {
            if (g_error_registered) {
                DriveEvent::RemoveListenerOnError(on_error_trampoline);
                g_error_registered = false;
            }
            g_on_error = py::function();
            return;
        }

        g_on_error = cb_obj.cast<py::function>();
        if (!g_error_registered) {
            DriveEvent::AddListenerOnError(on_error_trampoline);
            g_error_registered = true;
        }
    });

    m.def("on_state_change", [](py::object cb_obj) {
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (cb_obj.is_none()) {
            if (g_state_registered) {
                DriveEvent::RemoveListenerOnStateChange(on_state_change_trampoline);
                g_state_registered = false;
            }
            g_on_state_change = py::function();
            return;
        }

        g_on_state_change = cb_obj.cast<py::function>();
        if (!g_state_registered) {
            DriveEvent::AddListenerOnStateChange(on_state_change_trampoline);
            g_state_registered = true;
        }
    });

    // Explicit cleanup (does NOT dispose the library; just detaches listeners).
    m.def("clear_listeners", []() {
        // Called from Python -> GIL held.
        clear_listeners_impl();
    });

}
