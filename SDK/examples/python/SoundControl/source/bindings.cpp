#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include "SoundControl.h"
#include "SoundEvent.h"
#include "SoundEventListener.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_begin;
static py::function g_on_complete;
static py::function g_on_abort;
static py::function g_on_error;

static bool g_begin_registered = false;
static bool g_complete_registered = false;
static bool g_abort_registered = false;
static bool g_error_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// SoundEvent stores raw SoundEventListener* pointers. If Python garbage-collects the
// listener object while still registered, C++ can call a dangling pointer and crash.
// To prevent that, we keep strong references to Python listener objects until they
// are explicitly removed or cleared.
static std::mutex g_listener_mutex;
static std::vector<py::object> g_py_listeners;

template <typename... Args>
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

// Trampolines matching static SoundEvent signatures
static void on_begin_trampoline(uint16_t id, float volume)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_begin; }

    safe_call(cb, id, volume);
}

static void on_complete_trampoline(uint16_t id)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_complete; }

    safe_call(cb, id);
}

static void on_abort_trampoline(uint16_t id)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_abort; }

    safe_call(cb, id);
}

static void on_error_trampoline(uint16_t id)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_error; }

    safe_call(cb, id);
}

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PySoundEventListener : public SoundEventListener {
public:
    using SoundEventListener::SoundEventListener;

    void onSoundBegin(uint16_t id, float volume) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, SoundEventListener, onSoundBegin, id, volume);
    }

    void onSoundComplete(uint16_t id) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, SoundEventListener, onSoundComplete, id);
    }

    void onSoundAbort(uint16_t id) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, SoundEventListener, onSoundAbort, id);
    }

    void onSoundError(uint16_t id) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, SoundEventListener, onSoundError, id);
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

        if (g_begin_registered) {
            SoundEvent::RemoveListenerOnBegin(on_begin_trampoline);
            g_begin_registered = false;
        }
        if (g_complete_registered) {
            SoundEvent::RemoveListenerOnComplete(on_complete_trampoline);
            g_complete_registered = false;
        }
        if (g_abort_registered) {
            SoundEvent::RemoveListenerOnAbort(on_abort_trampoline);
            g_abort_registered = false;
        }
        if (g_error_registered) {
            SoundEvent::RemoveListenerOnError(on_error_trampoline);
            g_error_registered = false;
        }

        g_on_begin = py::function();
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
            auto *ptr = obj.cast<SoundEventListener*>();
            if (ptr) SoundEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

// -------------------------------
// Module
// -------------------------------
PYBIND11_MODULE(doly_sound, m) {
    m.doc() = "Bindings for SoundControl / SoundEvent (pybind11)";

    py::enum_<SoundState>(m, "SoundState")
        .value("Set", SoundState::SET)
        .value("Stop", SoundState::STOP)
        .value("Play", SoundState::PLAY);

    // Listener base class (object-based)
    py::class_<SoundEventListener, PySoundEventListener>(m, "SoundEventListener")
        .def(py::init<>())
        .def("onSoundBegin", &SoundEventListener::onSoundBegin)
        .def("onSoundComplete", &SoundEventListener::onSoundComplete)
        .def("onSoundAbort", &SoundEventListener::onSoundAbort)
        .def("onSoundError", &SoundEventListener::onSoundError);

    m.def(
        "add_listener",
        [](py::object listener_obj, bool priority) {
            auto *listener = listener_obj.cast<SoundEventListener*>();
            SoundEvent::AddListener(listener, priority);

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
            auto *listener = listener_obj.cast<SoundEventListener*>();
            SoundEvent::RemoveListener(listener);

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
        "on_begin",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_begin = std::move(cb);

            if (!g_begin_registered) {
                SoundEvent::AddListenerOnBegin(on_begin_trampoline);
                g_begin_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static begin callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_complete",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_complete = std::move(cb);

            if (!g_complete_registered) {
                SoundEvent::AddListenerOnComplete(on_complete_trampoline);
                g_complete_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static complete callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_abort",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_abort = std::move(cb);

            if (!g_abort_registered) {
                SoundEvent::AddListenerOnAbort(on_abort_trampoline);
                g_abort_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static abort callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_error",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_error = std::move(cb);

            if (!g_error_registered) {
                SoundEvent::AddListenerOnError(on_error_trampoline);
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
    // SoundControl API
    // -------------------------------
    m.def(
        "init",
        []() {
            g_callbacks_enabled.store(true, std::memory_order_relaxed);
            return SoundControl::init();
        },
        "Initialize sound control."
    );

    // Safer dispose: clear callbacks first
    m.def(
        "dispose",
        []() {
            clear_listeners_impl();
            return SoundControl::dispose();
        },
        "Dispose sound control (also clears static callbacks first)."
    );

    m.def(
        "play",
        &SoundControl::play,
        py::arg("file_name"),
        py::arg("block_id"),
        "Play a sound file, return 0 = success"
    );

    m.def("abort", &SoundControl::Abort, "Abort current playing sound.");

    m.def("get_state", &SoundControl::getState, "Get current sound state.");

    m.def("is_active", &SoundControl::isActive, "Return True if initialized/active.");

    m.def(
        "set_volume",
        &SoundControl::setVolume,
        py::arg("volume"),
        "Set volume as percentage (0-100)."
    );

    m.def("get_version", &SoundControl::getVersion, "Get version (0.XYZ).");
}
