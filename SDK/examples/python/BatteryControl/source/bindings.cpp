#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <atomic>
#include <algorithm>
#include <mutex>
#include <vector>

#include "BatteryControl.h"
#include "BatteryEvent.h"
#include "BatteryEventListener.h"

namespace py = pybind11;
using namespace pybind11::literals;

// ------------------------------
// Lifetime / thread-safety guards
// ------------------------------

static std::atomic<bool> g_callbacks_enabled{true};

static std::mutex g_cb_mutex;
static py::function g_on_alarm;
static bool g_alarm_registered = false;

// ------------------------------
// Lifetime safety for class-based listeners
// ------------------------------
// BatteryEvent stores raw BatteryEventListener* pointers. If Python garbage-
// collects a listener object while still registered, C++ can call a dangling
// pointer and crash. Keep strong references until explicitly removed.
static std::mutex g_listener_mutex;
static std::vector<py::object> g_py_listeners;

static inline void safe_call(const py::function &fn, auto&&... args) {
    if (!fn) return;
    try {
        fn(std::forward<decltype(args)>(args)...);
    } catch (const py::error_already_set&) {
        PyErr_Print();
    }
}

// -------------------------------
// Trampoline for Python overrides
// -------------------------------
class PyBatteryEventListener : public BatteryEventListener {
public:
    using BatteryEventListener::BatteryEventListener;

    void onBatteryAlarm(uint8_t capacity) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, BatteryEventListener, onBatteryAlarm, capacity);
    }

};

// Static callback trampolines.
static void on_alarm_trampoline(std::uint8_t capacity)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;
    py::function cb;
    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);
        cb = g_on_alarm;
    }
    safe_call(cb, capacity);
}

static void clear_listeners_impl(bool from_atexit = false)
{
    g_callbacks_enabled.store(false, std::memory_order_relaxed);

    {
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (g_alarm_registered) {
            BatteryEvent::RemoveListenerOnAlarm(on_alarm_trampoline);
            g_alarm_registered = false;
        }      

        g_on_alarm = py::function();
    }

    std::vector<py::object> listeners;
    {
        std::lock_guard<std::mutex> lk(g_listener_mutex);
        listeners.swap(g_py_listeners);
    }

    for (auto &obj : listeners) {
        try {
            auto *ptr = obj.cast<BatteryEventListener*>();
            if (ptr) BatteryEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore during teardown
        }
    }

    (void)from_atexit;
}

PYBIND11_MODULE(doly_battery, m)
{
    m.doc() = "Python wrapper for Doly BatteryControl (static C++ library)";

    py::class_<BatteryEventListener, PyBatteryEventListener>(m, "BatteryEventListener")
        .def(py::init<>(), "Create a BatteryEventListener base object.")
        .def("on_battery_alarm", &BatteryEventListener::onBatteryAlarm, "capacity"_a,
             "Called when battery capacity drops to or below the configured alarm threshold.");

    m.def("is_active", &BatteryControl::isActive,
          "Return true if BatteryControl is initialized and active.");

    m.def("init", []() {
        g_callbacks_enabled.store(true, std::memory_order_relaxed);
        return BatteryControl::init();
    }, "Initialize BatteryControl.");

    m.def("dispose", []() {
        clear_listeners_impl(false);
        return BatteryControl::dispose();
    }, "Dispose BatteryControl and unregister all listeners.");

    m.def("set_alarm_threshold", &BatteryControl::SetAlarmThreshold, "value"_a,
          "Set the battery alarm threshold in percent.");

    m.def("get_capacity", &BatteryControl::getCapacity,
          "Return the current battery capacity in percent.");

    m.def("get_version", &BatteryControl::getVersion,
          "Return the BatteryControl library version.");

    m.def("add_listener", [](py::object listener_obj, bool priority) {
        if (listener_obj.is_none()) return;

        auto *ptr = listener_obj.cast<BatteryEventListener*>();
        if (!ptr) return;

        {
            std::lock_guard<std::mutex> lk(g_listener_mutex);
            for (auto &o : g_py_listeners) {
                try {
                    if (o.cast<BatteryEventListener*>() == ptr) {
                        return;
                    }
                } catch (const py::cast_error&) {
                }
            }
            g_py_listeners.push_back(listener_obj);
        }

        BatteryEvent::AddListener(ptr, priority);
    }, "listener"_a, "priority"_a = false,
    "Register a class-based BatteryEventListener and keep it alive while registered.");

    m.def("remove_listener", [](py::object listener_obj) {
        if (listener_obj.is_none()) return;

        auto *ptr = listener_obj.cast<BatteryEventListener*>();
        if (!ptr) return;

        BatteryEvent::RemoveListener(ptr);

        std::lock_guard<std::mutex> lk(g_listener_mutex);
        g_py_listeners.erase(
            std::remove_if(g_py_listeners.begin(), g_py_listeners.end(),
                [ptr](const py::object &o) {
                    try {
                        return o.cast<BatteryEventListener*>() == ptr;
                    } catch (const py::cast_error&) {
                        return false;
                    }
                }),
            g_py_listeners.end());
    }, "listener"_a,
    "Unregister a class-based BatteryEventListener and drop its keepalive reference.");

    m.def("clear_listeners", []() {
        clear_listeners_impl(false);
    }, "Unregister all static and class-based listeners.");

    m.def("on_alarm", [](py::object cb) {
        g_callbacks_enabled.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_cb_mutex);

        if (cb.is_none()) {
            if (g_alarm_registered) {
                BatteryEvent::RemoveListenerOnAlarm(on_alarm_trampoline);
                g_alarm_registered = false;
            }
            g_on_alarm = py::function();
            return;
        }

        g_on_alarm = cb.cast<py::function>();
        if (!g_alarm_registered) {
            BatteryEvent::AddListenerOnAlarm(on_alarm_trampoline);
            g_alarm_registered = true;
        }
    }, "callback"_a,
    "Register a Python callback for battery alarm events. Pass None to unregister.");

    py::module_ atexit = py::module_::import("atexit");
    atexit.attr("register")(py::cpp_function([]() {
        clear_listeners_impl(true);
    }));
}
