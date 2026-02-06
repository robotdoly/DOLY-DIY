#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <mutex>
#include <atomic>
#include <vector>

#include "EdgeControl.h"
#include "EdgeEvent.h"
#include "EdgeEventListener.h"
#include "GpioPin.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_change;
static py::function g_on_gap_detect;

static bool g_change_registered = false;
static bool g_gap_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// EdgeEvent stores raw EdgeEventListener* pointers. If Python garbage-collects the
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

// Trampolines matching static EdgeEvent signatures
static void on_change_trampoline(std::vector<IrSensor> sensors)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_change; }

    safe_call(cb, sensors);
}

static void on_gap_detect_trampoline(GapDirection gap_type)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_gap_detect; }

    safe_call(cb, gap_type);
}

// -------------------------------
// Trampoline class for class-based listeners (Python subclassing)
// -------------------------------
struct PyEdgeEventListener : EdgeEventListener {
    using EdgeEventListener::EdgeEventListener;

    void onEdgeChange(std::vector<IrSensor> sensors) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, EdgeEventListener, onEdgeChange, sensors);
    }

    void onGapDetect(GapDirection gap_type) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, EdgeEventListener, onGapDetect, gap_type);
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

        if (g_change_registered) {
            EdgeEvent::RemoveListenerOnChange(on_change_trampoline);
            g_change_registered = false;
        }
        if (g_gap_registered) {
            EdgeEvent::RemoveListenerOnGapDetect(on_gap_detect_trampoline);
            g_gap_registered = false;
        }

        g_on_change = py::function();
        g_on_gap_detect = py::function();
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
            auto *ptr = obj.cast<EdgeEventListener*>();
            if (ptr) EdgeEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

// -------------------------------
// Module
// -------------------------------
PYBIND11_MODULE(doly_edge, m) {
    // Enums / structs
    py::enum_<GpioState>(m, "GpioState")
        .value("Low", GpioState::LOW)
        .value("High", GpioState::HIGH);

    py::enum_<GapDirection>(m, "GapDirection")
        .value("Front", GapDirection::FRONT)
        .value("Front_Left", GapDirection::FRONT_LEFT)
        .value("Front_Right", GapDirection::FRONT_RIGHT)
        .value("Back", GapDirection::BACK)
        .value("Back_Left", GapDirection::BACK_LEFT)
        .value("Back_Right", GapDirection::BACK_RIGHT)
        .value("Left", GapDirection::LEFT)
        .value("Right", GapDirection::RIGHT)
        .value("Cross_Left", GapDirection::CROSS_LEFT)
        .value("Cross_Right", GapDirection::CROSS_RIGHT)
        .value("All", GapDirection::ALL);

    py::enum_<SensorId>(m, "SensorId")
        .value("Back_Left", SensorId::BACK_LEFT)
        .value("Back_Right", SensorId::BACK_RIGHT)
        .value("Front_Left", SensorId::FRONT_LEFT)
        .value("Front_Right", SensorId::FRONT_RIGHT);

    py::class_<IrSensor>(m, "IrSensor")
        .def(py::init<>())
        .def_readwrite("id", &IrSensor::id)
        .def_readwrite("state", &IrSensor::state)
        .def("__repr__", [](const IrSensor& s) {
            return "<IrSensor id=" + std::to_string((int)s.id) +
            " state=" + std::to_string((int)s.state) + ">";
             });

    // Class-based listener interface (subclass in Python)
    py::class_<EdgeEventListener, PyEdgeEventListener>(m, "EdgeEventListener")
        .def(py::init<>())
        .def("onEdgeChange", &EdgeEventListener::onEdgeChange)
        .def("onGapDetect", &EdgeEventListener::onGapDetect);

    // -------------------------------
    // Class-based listener management
    // -------------------------------
    m.def("add_listener",
          [](py::object listener_obj, bool priority) {
              auto *listener = listener_obj.cast<EdgeEventListener*>();
              EdgeEvent::AddListener(listener, priority);

              std::lock_guard<std::mutex> lock(g_listener_mutex);
              auto it = std::find_if(g_py_listeners.begin(), g_py_listeners.end(),
                                     [&](const py::object &o) { return o.is(listener_obj); });
              if (it == g_py_listeners.end()) {
                  g_py_listeners.push_back(listener_obj);
              }
          },
          py::arg("listener"), py::arg("priority") = false,
          "Register a class-based EdgeEventListener.\n"
          "The binding keeps a strong reference to prevent GC while C++ holds the raw pointer.");

    m.def("remove_listener",
          [](py::object listener_obj) {
              auto *listener = listener_obj.cast<EdgeEventListener*>();
              EdgeEvent::RemoveListener(listener);

              std::lock_guard<std::mutex> lock(g_listener_mutex);
              g_py_listeners.erase(
                  std::remove_if(g_py_listeners.begin(), g_py_listeners.end(),
                                 [&](const py::object &o) { return o.is(listener_obj); }),
                  g_py_listeners.end());
          },
          py::arg("listener"),
          "Unregister a previously-added class-based listener and release keepalive ref.");

    m.def("clear_listeners",
          []() { clear_listeners_impl(); },
          "Unregister all static callbacks and class-based listeners tracked by the binding.");

    // -------------------------------
    // Static callbacks (one per event type)
    // -------------------------------
    m.def("on_change",
          [](py::function cb) {
              std::lock_guard<std::mutex> lock(g_cb_mutex);
              g_callbacks_enabled.store(true, std::memory_order_relaxed);

              g_on_change = std::move(cb);

              if (!g_change_registered) {
                  EdgeEvent::AddListenerOnChange(on_change_trampoline);
                  g_change_registered = true;
              }
          },
          py::arg("cb"),
          "Set a static on-change callback (replaces any previous one).\n"
          "Tip: If you want multiple handlers, use a Python dispatcher function.");

    m.def("on_gap_detect",
          [](py::function cb) {
              std::lock_guard<std::mutex> lock(g_cb_mutex);
              g_callbacks_enabled.store(true, std::memory_order_relaxed);

              g_on_gap_detect = std::move(cb);

              if (!g_gap_registered) {
                  EdgeEvent::AddListenerOnGapDetect(on_gap_detect_trampoline);
                  g_gap_registered = true;
              }
          },
          py::arg("cb"),
          "Set a static gap-detect callback (replaces any previous one).\n"
          "Tip: If you want multiple handlers, use a Python dispatcher function.");

    // Optional: allow clearing static callbacks by passing None
    m.def("clear_callbacks",
          []() {
              py::gil_scoped_acquire gil;
              std::lock_guard<std::mutex> lock(g_cb_mutex);

              if (g_change_registered) {
                  EdgeEvent::RemoveListenerOnChange(on_change_trampoline);
                  g_change_registered = false;
              }
              if (g_gap_registered) {
                  EdgeEvent::RemoveListenerOnGapDetect(on_gap_detect_trampoline);
                  g_gap_registered = false;
              }

              g_on_change = py::function();
              g_on_gap_detect = py::function();
          },
          "Unregister static callback trampolines and clear stored Python callables.");

    // -------------------------------
    // EdgeControl API (namespace functions)
    // -------------------------------
    m.def("init", []() { return EdgeControl::init(); },
          "Initialize edge control.\nReturns 0 on success, negative on error.");

    m.def("dispose",
          []() {
              // Ensure we won't call into Python after disposal
              clear_listeners_impl();

              // Call underlying lib dispose
              return EdgeControl::dispose();
          },
          "Dispose edge control and release resources.\n"
          "Also clears listeners tracked by the binding to avoid shutdown races.");

    m.def("is_active", []() { return EdgeControl::isActive(); },
          "Return True if edge control is initialized and active.");

    m.def("enable_control", []() { return EdgeControl::enableControl(); },
          "Start the sensor listening thread.\nReturns 0 on success, 1 if already running.");

    m.def("disable_control", []() { return EdgeControl::disableControl(); },
          "Stop the sensor listening thread.\nReturns 0 on success, 1 if not running.");

    m.def("get_sensors",
          [](GpioState state) { return EdgeControl::getSensors(state); },
          py::arg("state"),
          "Get IR sensors filtered by GPIO state.");

    m.def("get_version", []() { return EdgeControl::getVersion(); },
          "Return library version as float (format 0.XYZ).");
   
}
