#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include "TofControl.h"
#include "TofEvent.h"
#include "TofEventListener.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_proximity_gesture;
static py::function g_on_proximity_threshold;

static bool g_gesture_registered = false;
static bool g_threshold_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// TofEvent stores raw TofEventListener* pointers. If Python garbage-collects the
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

// Trampolines matching static TofEvent signatures
static void on_proximity_gesture_trampoline(TofGesture left, TofGesture right)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_proximity_gesture; }

    safe_call(cb, left, right);
}

static void on_proximity_threshold_trampoline(TofData left, TofData right)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_proximity_threshold; }

    safe_call(cb, left, right);
}

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyTofEventListener : public TofEventListener {
public:
    using TofEventListener::TofEventListener;

    void onProximityGesture(TofGesture left, TofGesture right) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, TofEventListener, onProximityGesture, left, right);
    }

    void onProximityThreshold(TofData left, TofData right) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, TofEventListener, onProximityThreshold, left, right);
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

        if (g_gesture_registered) {
            TofEvent::RemoveListenerOnProximityGesture(on_proximity_gesture_trampoline);
            g_gesture_registered = false;
        }
        if (g_threshold_registered) {
            TofEvent::RemoveListenerOnProximityThreshold(on_proximity_threshold_trampoline);
            g_threshold_registered = false;
        }

        g_on_proximity_gesture = py::function();
        g_on_proximity_threshold = py::function();
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
            auto *ptr = obj.cast<TofEventListener*>();
            if (ptr) TofEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

PYBIND11_MODULE(doly_tof, m) {
    m.doc() = "Bindings for TofControl / TofEvent (pybind11)";

    // -------------------------------
    // Enums + data structs
    // -------------------------------
    py::enum_<TofError>(m, "TofError")
        .value("NoError", TofError::NO_ERROR)
        .value("VcselContinuityTest", TofError::VCSEL_Continuity_Test)
        .value("VcselWatchdogTest", TofError::VCSEL_Watchdog_Test)
        .value("VcselWatchdog", TofError::VCSEL_Watchdog)
        .value("Pll1Lock", TofError::PLL1_Lock)
        .value("Pll2Lock", TofError::PLL2_Lock)
        .value("EarlyConvergenceEstimate", TofError::Early_Convergence_Estimate)
        .value("MaxConvergence", TofError::Max_Convergence)
        .value("NoTargetIgnore", TofError::No_Target_Ignore)
        .value("MaxSignalToNoiseRatio", TofError::Max_Signal_To_Noise_Ratio)
        .value("RawRangingAlgoUnderflow", TofError::Raw_Ranging_Algo_Underflow)
        .value("RawRangingAlgoOverflow", TofError::Raw_Ranging_Algo_Overflow)
        .value("RangingAlgoUnderflow", TofError::Ranging_Algo_Underflow)
        .value("RangingAlgoOverflow", TofError::Ranging_Algo_Overflow)
        .value("FilteredByPostProcessing", TofError::Filtered_by_post_processing)
        .value("DataNotReady", TofError::DataNotReady);

    py::enum_<TofSide>(m, "TofSide")
        .value("Left", TofSide::LEFT)
        .value("Right", TofSide::RIGHT);

    py::enum_<TofGestureType>(m, "TofGestureType")
        .value("Undefined", TofGestureType::UNDEFINED)
        .value("ObjectComing", TofGestureType::OBJECT_COMING)
        .value("ObjectGoing", TofGestureType::OBJECT_GOING)
        .value("Scrubing", TofGestureType::SCRUBING)
        .value("ToLeft", TofGestureType::TO_LEFT)
        .value("ToRight", TofGestureType::TO_RIGHT);

    py::class_<TofGesture>(m, "TofGesture")
        .def(py::init<>())
        .def_readwrite("type", &TofGesture::type)
        .def_readwrite("range_mm", &TofGesture::range_mm);

    py::class_<TofData>(m, "TofData")
        .def(py::init<>())
        .def_readwrite("update_ms", &TofData::update_ms)
        .def_readwrite("range_mm", &TofData::range_mm)
        .def_readwrite("error", &TofData::error)
        .def_readwrite("side", &TofData::side);

    // -------------------------------
    // Listener base class (object-based)
    // -------------------------------
    py::class_<TofEventListener, PyTofEventListener>(m, "TofEventListener")
        .def(py::init<>())
        .def("onProximityGesture", &TofEventListener::onProximityGesture)
        .def("onProximityThreshold", &TofEventListener::onProximityThreshold);

    m.def(
        "add_listener",
        [](py::object listener_obj, bool priority) {
            // Register with the C++ event system
            auto *listener = listener_obj.cast<TofEventListener*>();
            TofEvent::AddListener(listener, priority);

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
            auto *listener = listener_obj.cast<TofEventListener*>();
            TofEvent::RemoveListener(listener);

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
        "on_proximity_gesture",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            // Replace callback
            g_on_proximity_gesture = std::move(cb);

            // Ensure trampoline is registered exactly once
            if (!g_gesture_registered) {
                TofEvent::AddListenerOnProximityGesture(on_proximity_gesture_trampoline);
                g_gesture_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static proximity-gesture callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_proximity_threshold",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            // Replace callback
            g_on_proximity_threshold = std::move(cb);

            // Ensure trampoline is registered exactly once
            if (!g_threshold_registered) {
                TofEvent::AddListenerOnProximityThreshold(on_proximity_threshold_trampoline);
                g_threshold_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static proximity-threshold callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "clear_listeners",
        []() { clear_listeners_impl(); },
        "Unregister all static callbacks and clear stored Python functions."
    );

    // -------------------------------
    // TofControl API
    // -------------------------------
    m.def(
        "init",
        [](int8_t offset_left, int8_t offset_right) {
            g_callbacks_enabled.store(true, std::memory_order_relaxed);
            return TofControl::init(offset_left, offset_right);
        },
        py::arg("offset_left") = 0, py::arg("offset_right") = 0,
        "Initialize ToF control. Returns 0 on success."
    );

    // Safer dispose: clear callbacks first
    m.def(
        "dispose",
        []() {
            clear_listeners_impl();
            return TofControl::dispose();
        },
        "Dispose ToF control (also clears static callbacks first)."
    );

    m.def(
        "setup_continuous",
        &TofControl::setup_continuous,
        py::arg("interval_ms") = 50, py::arg("distance") = 0,
        "Configure continuous reading. interval_ms max 2550. distance 0 disables threshold event."
    );

    m.def(
        "get_sensors_data",
        &TofControl::getSensorsData,
        "Get current left/right sensor data."
    );

    m.def("is_active", &TofControl::isActive, "Return True if initialized/active.");
    m.def("is_reading", &TofControl::isReading, "Return True if reading continuously.");
    m.def("get_version", &TofControl::getVersion, "Get version (0.XYZ).");
}
