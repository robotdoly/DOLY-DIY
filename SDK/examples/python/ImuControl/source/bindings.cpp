#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include "ImuControl.h"
#include "ImuEvent.h"
#include "ImuEventListener.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_update;
static py::function g_on_gesture;

static bool g_update_registered = false;
static bool g_gesture_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// ImuEvent stores raw ImuEventListener* pointers. If Python garbage-collects the
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

// Trampolines matching static ImuEvent signatures
static void on_update_trampoline(ImuData data)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_update; }

    safe_call(cb, data);
}

static void on_gesture_trampoline(ImuGesture type, GestureDirection from)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_gesture; }

    safe_call(cb, type, from);
}

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyImuEventListener : public ImuEventListener {
public:
    using ImuEventListener::ImuEventListener;

    void onImuUpdate(ImuData data) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ImuEventListener, onImuUpdate, data);
    }

    void onImuGesture(ImuGesture type, GestureDirection from) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, ImuEventListener, onImuGesture, type, from);
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

        if (g_update_registered) {
            ImuEvent::RemoveListenerUpdateEvent(on_update_trampoline);
            g_update_registered = false;
        }
        if (g_gesture_registered) {
            ImuEvent::RemoveListenerGestureEvent(on_gesture_trampoline);
            g_gesture_registered = false;
        }

        g_on_update = py::function();
        g_on_gesture = py::function();
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
            auto *ptr = obj.cast<ImuEventListener*>();
            if (ptr) ImuEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

// -------------------------------
// Module
// -------------------------------
PYBIND11_MODULE(doly_imu, m) {
    m.doc() = "Bindings for ImuControl / ImuEvent (pybind11)";

    py::enum_<ImuGesture>(m, "ImuGesture")
        .value("Undefined", ImuGesture::UNDEFINED)
        .value("Move", ImuGesture::MOVE)
        .value("LongShake", ImuGesture::LONG_SHAKE)
        .value("ShortShake", ImuGesture::SHORT_SHAKE)
        .value("Vibrate", ImuGesture::VIBRATE)
        .value("VibrateExtreme", ImuGesture::VIBRATE_EXTREME)
        .value("ShockLight", ImuGesture::SHOCK_LIGHT)
        .value("ShockMedium", ImuGesture::SHOCK_MEDIUM)
        .value("ShockHard", ImuGesture::SHOCK_HARD)
        .value("ShockExtreme", ImuGesture::SHOCK_EXTREME);

    py::enum_<GestureDirection>(m, "GestureDirection")
        .value("Left", GestureDirection::LEFT)
        .value("Right", GestureDirection::RIGHT)
        .value("Up", GestureDirection::UP)
        .value("Down", GestureDirection::DOWN)
        .value("Front", GestureDirection::FRONT)
        .value("Back", GestureDirection::BACK);

    py::class_<VectorFloat>(m, "VectorFloat")
        .def(py::init<>())
        .def_readwrite("x", &VectorFloat::x)
        .def_readwrite("y", &VectorFloat::y)
        .def_readwrite("z", &VectorFloat::z);

    py::class_<YawPitchRoll>(m, "YawPitchRoll")
        .def(py::init<>())
        .def_readwrite("yaw", &YawPitchRoll::yaw)
        .def_readwrite("pitch", &YawPitchRoll::pitch)
        .def_readwrite("roll", &YawPitchRoll::roll);

    py::class_<ImuData>(m, "ImuData")
        .def(py::init<>())
        .def_readwrite("ypr", &ImuData::ypr)
        .def_readwrite("linear_accel", &ImuData::linear_accel)
        .def_readwrite("temperature", &ImuData::temperature);

    // Listener base class (object-based)
    py::class_<ImuEventListener, PyImuEventListener>(m, "ImuEventListener")
        .def(py::init<>())
        .def("onImuUpdate", &ImuEventListener::onImuUpdate)
        .def("onImuGesture", &ImuEventListener::onImuGesture);

    m.def(
        "add_listener",
        [](py::object listener_obj, bool priority) {
            auto *listener = listener_obj.cast<ImuEventListener*>();
            ImuEvent::AddListener(listener, priority);

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
            auto *listener = listener_obj.cast<ImuEventListener*>();
            ImuEvent::RemoveListener(listener);

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
        "on_update",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_update = std::move(cb);
            if (!g_update_registered) {
                ImuEvent::AddListenerUpdateEvent(on_update_trampoline);
                g_update_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static IMU update callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_gesture",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_gesture = std::move(cb);
            if (!g_gesture_registered) {
                ImuEvent::AddListenerGestureEvent(on_gesture_trampoline);
                g_gesture_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static IMU gesture callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "clear_listeners",
        []() { clear_listeners_impl(); },
        "Unregister all static callbacks and clear stored Python functions."
    );

    // -------------------------------
    // ImuControl API
    // -------------------------------
    m.def(
        "init",
        [](uint8_t delay, int16_t gx, int16_t gy, int16_t gz, int16_t ax, int16_t ay, int16_t az) {
            g_callbacks_enabled.store(true, std::memory_order_relaxed);
            return ImuControl::init(delay, gx, gy, gz, ax, ay, az);
        },
        py::arg("delay") = 0,
        py::arg("gx") = 0, py::arg("gy") = 0, py::arg("gz") = 0,
        py::arg("ax") = 0, py::arg("ay") = 0, py::arg("az") = 0,
        "Initialize the IMU.\n"
        "delay: delay in ms before processing events.\n"
        "gx/gy/gz: gyro offsets, ax/ay/az: accel offsets."
    );

    m.def(
        "dispose",
        []() {
            clear_listeners_impl();
            return ImuControl::dispose();
        },
        "Dispose IMU (also clears callbacks/listeners first)."
    );

    m.def(
        "calculate_offsets",
        []() {
            int16_t gx = 0, gy = 0, gz = 0, ax = 0, ay = 0, az = 0;
            int8_t rc = ImuControl::calculate_offsets(&gx, &gy, &gz, &ax, &ay, &az);
            return py::make_tuple(rc, gx, gy, gz, ax, ay, az);
        },
        "Calculate IMU offsets.\n"
        "Returns (result, gx, gy, gz, ax, ay, az)."
    );

    m.def("get_imu_data", &ImuControl::getImuData, "Return latest IMU reading.");
    m.def("get_temperature", &ImuControl::getTemperature, "Return IMU temperature.");
    m.def("get_version", &ImuControl::getVersion, "Get version (0.XYZ).");
}
