#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include "EyeControl.h"
#include "EyeEvent.h"
#include "EyeEventListener.h"

namespace py = pybind11;

// -------------------------------
// Static callback storage (single callback per event type)
// -------------------------------
static std::mutex g_cb_mutex;

static py::function g_on_start;
static py::function g_on_complete;
static py::function g_on_abort;

static bool g_start_registered = false;
static bool g_complete_registered = false;
static bool g_abort_registered = false;

// Helps avoid calling into Python during shutdown/dispose races
static std::atomic<bool> g_callbacks_enabled{true};

// -------------------------------
// Lifetime safety for class-based listeners
// -------------------------------
// EyeEvent stores raw EyeEventListener* pointers. If Python garbage-collects the
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

// Trampolines matching static EyeEvent signatures
static void on_start_trampoline(uint16_t id)
{
    if (!g_callbacks_enabled.load(std::memory_order_relaxed)) return;

    py::gil_scoped_acquire gil;

    py::function cb;
    { std::lock_guard<std::mutex> lock(g_cb_mutex); cb = g_on_start; }

    safe_call(cb, id);
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

// -------------------------------
// Trampoline for Python overrides (class-based listener)
// -------------------------------
class PyEyeEventListener : public EyeEventListener {
public:
    using EyeEventListener::EyeEventListener;

    void onEyeStart(uint16_t id) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, EyeEventListener, onEyeStart, id);
    }

    void onEyeComplete(uint16_t id) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, EyeEventListener, onEyeComplete, id);
    }

    void onEyeAbort(uint16_t id) override {
        py::gil_scoped_acquire gil;
        PYBIND11_OVERRIDE(void, EyeEventListener, onEyeAbort, id);
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

        if (g_start_registered) {
            EyeEvent::RemoveListenerOnStart(on_start_trampoline);
            g_start_registered = false;
        }
        if (g_complete_registered) {
            EyeEvent::RemoveListenerOnComplete(on_complete_trampoline);
            g_complete_registered = false;
        }
        if (g_abort_registered) {
            EyeEvent::RemoveListenerOnAbort(on_abort_trampoline);
            g_abort_registered = false;
        }

        g_on_start = py::function();
        g_on_complete = py::function();
        g_on_abort = py::function();
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
            auto *ptr = obj.cast<EyeEventListener*>();
            if (ptr) EyeEvent::RemoveListener(ptr);
        } catch (const py::cast_error&) {
            // ignore
        }
    }
}

PYBIND11_MODULE(doly_eye, m) {
    m.doc() = "Bindings for EyeControl / EyeEvent (pybind11)";

    // Shared types (Color / ColorCode) live in doly_color
    py::module_::import("doly_color");

    // Shared types (VContent) live in doly_vcontent
    py::module_::import("doly_vcontent");

    py::enum_<EyeSide>(m, "EyeSide")
        .value("Both", EyeSide::BOTH)
        .value("Left", EyeSide::LEFT)
        .value("Right", EyeSide::RIGHT);

    py::enum_<IrisShape>(m, "IrisShape")
        .value("Classic", IrisShape::CLASSIC)
        .value("Modern", IrisShape::MODERN)
        .value("Space", IrisShape::SPACE)
        .value("Orbit", IrisShape::ORBIT)
        .value("Glow", IrisShape::GLOW)
        .value("Digi", IrisShape::DIGI);

    // Listener base class (object-based)
    py::class_<EyeEventListener, PyEyeEventListener>(m, "EyeEventListener")
        .def(py::init<>())
        .def("onEyeStart", &EyeEventListener::onEyeStart)
        .def("onEyeComplete", &EyeEventListener::onEyeComplete)
        .def("onEyeAbort", &EyeEventListener::onEyeAbort);

    m.def(
        "add_listener",
        [](py::object listener_obj, bool priority) {
            auto *listener = listener_obj.cast<EyeEventListener*>();
            EyeEvent::AddListener(listener, priority);

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
            auto *listener = listener_obj.cast<EyeEventListener*>();
            EyeEvent::RemoveListener(listener);

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
        "on_start",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_start = std::move(cb);
            if (!g_start_registered) {
                EyeEvent::AddListenerOnStart(on_start_trampoline);
                g_start_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static eye-start callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_complete",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_complete = std::move(cb);
            if (!g_complete_registered) {
                EyeEvent::AddListenerOnComplete(on_complete_trampoline);
                g_complete_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static eye-complete callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "on_abort",
        [](py::function cb) {
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            g_callbacks_enabled.store(true, std::memory_order_relaxed);

            g_on_abort = std::move(cb);
            if (!g_abort_registered) {
                EyeEvent::AddListenerOnAbort(on_abort_trampoline);
                g_abort_registered = true;
            }
        },
        py::arg("cb"),
        "Set a static eye-abort callback (replaces any previous one).\n"
        "Tip: If you want multiple handlers, use a Python dispatcher function."
    );

    m.def(
        "clear_listeners",
        []() { clear_listeners_impl(); },
        "Unregister all static callbacks and clear stored Python functions."
    );

    // -------------------------------
    // Eye expressions (names for set_animation)
    // -------------------------------
    py::module_ expr = m.def_submodule("expressions", "Eye expression names.");
    expr.attr("ADMIRING") = py::str(EyeExpressions::ADMIRING);
    expr.attr("AGGRAVATED") = py::str(EyeExpressions::AGGRAVATED);
    expr.attr("ANNOYED") = py::str(EyeExpressions::ANNOYED);
    expr.attr("ANXIOUS") = py::str(EyeExpressions::ANXIOUS);
    expr.attr("ATTENTION") = py::str(EyeExpressions::ATTENTION);
    expr.attr("ATTENTION_LEFT") = py::str(EyeExpressions::ATTENTION_LEFT);
    expr.attr("ATTENTION_RIGHT") = py::str(EyeExpressions::ATTENTION_RIGHT);
    expr.attr("AWAKE_L") = py::str(EyeExpressions::AWAKE_L);
    expr.attr("AWAKE_R") = py::str(EyeExpressions::AWAKE_R);
    expr.attr("BATTERY_LOW") = py::str(EyeExpressions::BATTERY_LOW);
    expr.attr("BLINK") = py::str(EyeExpressions::BLINK);
    expr.attr("BLINK_BIG") = py::str(EyeExpressions::BLINK_BIG);
    expr.attr("BLINK_L") = py::str(EyeExpressions::BLINK_L);
    expr.attr("BLINK_ONLY") = py::str(EyeExpressions::BLINK_ONLY);
    expr.attr("BLINK_R") = py::str(EyeExpressions::BLINK_R);
    expr.attr("BLINK_SLOW") = py::str(EyeExpressions::BLINK_SLOW);
    expr.attr("BLUE") = py::str(EyeExpressions::BLUE);
    expr.attr("BUGGED") = py::str(EyeExpressions::BUGGED);
    expr.attr("BUMP") = py::str(EyeExpressions::BUMP);
    expr.attr("BURNED_UP") = py::str(EyeExpressions::BURNED_UP);
    expr.attr("CAUTIOUS") = py::str(EyeExpressions::CAUTIOUS);
    expr.attr("CAUTIOUS_DOWN") = py::str(EyeExpressions::CAUTIOUS_DOWN);
    expr.attr("CAUTIOUS_DOWN_LEFT") = py::str(EyeExpressions::CAUTIOUS_DOWN_LEFT);
    expr.attr("CAUTIOUS_DOWN_RIGHT") = py::str(EyeExpressions::CAUTIOUS_DOWN_RIGHT);
    expr.attr("CAUTIOUS_LEFT") = py::str(EyeExpressions::CAUTIOUS_LEFT);
    expr.attr("CAUTIOUS_RIGHT") = py::str(EyeExpressions::CAUTIOUS_RIGHT);
    expr.attr("CAUTIOUS_UP") = py::str(EyeExpressions::CAUTIOUS_UP);
    expr.attr("CHAOTIC") = py::str(EyeExpressions::CHAOTIC);
    expr.attr("CHEERFUL") = py::str(EyeExpressions::CHEERFUL);
    expr.attr("COLOR_CHANGE") = py::str(EyeExpressions::COLOR_CHANGE);
    expr.attr("CONCENTRATE") = py::str(EyeExpressions::CONCENTRATE);
    expr.attr("CONFUSED") = py::str(EyeExpressions::CONFUSED);
    expr.attr("CRAZY_ABOUT") = py::str(EyeExpressions::CRAZY_ABOUT);
    expr.attr("CRUSHED") = py::str(EyeExpressions::CRUSHED);
    expr.attr("CYCLOPS_L") = py::str(EyeExpressions::CYCLOPS_L);
    expr.attr("CYCLOPS_R") = py::str(EyeExpressions::CYCLOPS_R);
    expr.attr("DAMAGED") = py::str(EyeExpressions::DAMAGED);
    expr.attr("DEJECTED") = py::str(EyeExpressions::DEJECTED);
    expr.attr("DELIGHTED") = py::str(EyeExpressions::DELIGHTED);
    expr.attr("DEMORALIZED") = py::str(EyeExpressions::DEMORALIZED);
    expr.attr("DEPRESSED") = py::str(EyeExpressions::DEPRESSED);
    expr.attr("DESTROYED") = py::str(EyeExpressions::DESTROYED);
    expr.attr("DISCOVER") = py::str(EyeExpressions::DISCOVER);
    expr.attr("DISAPPOINTED") = py::str(EyeExpressions::DISAPPOINTED);
    expr.attr("DIZZY_L") = py::str(EyeExpressions::DIZZY_L);
    expr.attr("DIZZY_R") = py::str(EyeExpressions::DIZZY_R);
    expr.attr("DOWN") = py::str(EyeExpressions::DOWN);
    expr.attr("DROP_CENTER") = py::str(EyeExpressions::DROP_CENTER);
    expr.attr("DROWSY") = py::str(EyeExpressions::DROWSY);
    expr.attr("ELEVATED_I") = py::str(EyeExpressions::ELEVATED_I);
    expr.attr("ELEVATED_O") = py::str(EyeExpressions::ELEVATED_O);
    expr.attr("EMPTY") = py::str(EyeExpressions::EMPTY);
    expr.attr("EXCITED") = py::str(EyeExpressions::EXCITED);
    expr.attr("FED_UP") = py::str(EyeExpressions::FED_UP);
    expr.attr("FINE") = py::str(EyeExpressions::FINE);
    expr.attr("FIREMAN") = py::str(EyeExpressions::FIREMAN);
    expr.attr("FLAME") = py::str(EyeExpressions::FLAME);
    expr.attr("FOCUS") = py::str(EyeExpressions::FOCUS);
    expr.attr("FRIGHTENED") = py::str(EyeExpressions::FRIGHTENED);
    expr.attr("FRUSTRATED") = py::str(EyeExpressions::FRUSTRATED);
    expr.attr("FUMING_L") = py::str(EyeExpressions::FUMING_L);
    expr.attr("FUMING_R") = py::str(EyeExpressions::FUMING_R);
    expr.attr("FURIOUS") = py::str(EyeExpressions::FURIOUS);
    expr.attr("GET_BIGGER") = py::str(EyeExpressions::GET_BIGGER);
    expr.attr("GET_SMALLER") = py::str(EyeExpressions::GET_SMALLER);
    expr.attr("HAPPY") = py::str(EyeExpressions::HAPPY);
    expr.attr("HEARTS") = py::str(EyeExpressions::HEARTS);
    expr.attr("HEATED") = py::str(EyeExpressions::HEATED);
    expr.attr("HOPELESS") = py::str(EyeExpressions::HOPELESS);
    expr.attr("HOSTILE") = py::str(EyeExpressions::HOSTILE);
    expr.attr("IMPACT_F") = py::str(EyeExpressions::IMPACT_F);
    expr.attr("IMPACT_L") = py::str(EyeExpressions::IMPACT_L);
    expr.attr("IMPACT_R") = py::str(EyeExpressions::IMPACT_R);
    expr.attr("IMPATIENT") = py::str(EyeExpressions::IMPATIENT);
    expr.attr("INJURED") = py::str(EyeExpressions::INJURED);
    expr.attr("IRRITATED") = py::str(EyeExpressions::IRRITATED);
    expr.attr("JEALOUS_L") = py::str(EyeExpressions::JEALOUS_L);
    expr.attr("JEALOUS_R") = py::str(EyeExpressions::JEALOUS_R);
    expr.attr("LEFT") = py::str(EyeExpressions::LEFT);
    expr.attr("LIDS_DOWN_5S") = py::str(EyeExpressions::LIDS_DOWN_5S);
    expr.attr("LOOK_AHEAD") = py::str(EyeExpressions::LOOK_AHEAD);
    expr.attr("LOOK_AHEAD_SLOW") = py::str(EyeExpressions::LOOK_AHEAD_SLOW);
    expr.attr("LOOK_DOWN") = py::str(EyeExpressions::LOOK_DOWN);
    expr.attr("LOOK_DOWN_L") = py::str(EyeExpressions::LOOK_DOWN_L);
    expr.attr("LOOK_DOWN_R") = py::str(EyeExpressions::LOOK_DOWN_R);
    expr.attr("LOOK_LEFT") = py::str(EyeExpressions::LOOK_LEFT);
    expr.attr("LOOK_RIGHT") = py::str(EyeExpressions::LOOK_RIGHT);
    expr.attr("LOOK_UP") = py::str(EyeExpressions::LOOK_UP);
    expr.attr("LOOK_UP_L") = py::str(EyeExpressions::LOOK_UP_L);
    expr.attr("LOOK_UP_R") = py::str(EyeExpressions::LOOK_UP_R);
    expr.attr("LOW") = py::str(EyeExpressions::LOW);
    expr.attr("LOW_LONG") = py::str(EyeExpressions::LOW_LONG);
    expr.attr("MELANCHOLY") = py::str(EyeExpressions::MELANCHOLY);
    expr.attr("MID_DOWN") = py::str(EyeExpressions::MID_DOWN);
    expr.attr("MID_UP") = py::str(EyeExpressions::MID_UP);
    expr.attr("MID_UP_L") = py::str(EyeExpressions::MID_UP_L);
    expr.attr("MID_UP_R") = py::str(EyeExpressions::MID_UP_R);
    expr.attr("MIXED_UP") = py::str(EyeExpressions::MIXED_UP);
    expr.attr("MOODY_LR") = py::str(EyeExpressions::MOODY_LR);
    expr.attr("MOODY_RL") = py::str(EyeExpressions::MOODY_RL);
    expr.attr("NERVOUS") = py::str(EyeExpressions::NERVOUS);
    expr.attr("OFFENDED") = py::str(EyeExpressions::OFFENDED);
    expr.attr("OUTRAGED") = py::str(EyeExpressions::OUTRAGED);
    expr.attr("OVERJOYED") = py::str(EyeExpressions::OVERJOYED);
    expr.attr("PANICKY") = py::str(EyeExpressions::PANICKY);
    expr.attr("PASSIONATE") = py::str(EyeExpressions::PASSIONATE);
    expr.attr("PHOTO") = py::str(EyeExpressions::PHOTO);
    expr.attr("POLICE") = py::str(EyeExpressions::POLICE);
    expr.attr("PUZZLED") = py::str(EyeExpressions::PUZZLED);
    expr.attr("PUMPKIN") = py::str(EyeExpressions::PUMPKIN);
    expr.attr("RIGHT") = py::str(EyeExpressions::RIGHT);
    expr.attr("ROLL_UD") = py::str(EyeExpressions::ROLL_UD);
    expr.attr("SCAN") = py::str(EyeExpressions::SCAN);
    expr.attr("SHAKE_FRONT") = py::str(EyeExpressions::SHAKE_FRONT);
    expr.attr("SHAKE_LR") = py::str(EyeExpressions::SHAKE_LR);
    expr.attr("SHAKE_UD") = py::str(EyeExpressions::SHAKE_UD);
    expr.attr("SHOCKED") = py::str(EyeExpressions::SHOCKED);
    expr.attr("SHY") = py::str(EyeExpressions::SHY);
    expr.attr("SLEEP") = py::str(EyeExpressions::SLEEP);
    expr.attr("SLEEPY") = py::str(EyeExpressions::SLEEPY);
    expr.attr("SNEEZE") = py::str(EyeExpressions::SNEEZE);
    expr.attr("SPARKLING") = py::str(EyeExpressions::SPARKLING);
    expr.attr("SQUINT_RL") = py::str(EyeExpressions::SQUINT_RL);
    expr.attr("STORMING") = py::str(EyeExpressions::STORMING);
    expr.attr("SUNGLASS") = py::str(EyeExpressions::SUNGLASS);
    expr.attr("SUNGLASS_LR") = py::str(EyeExpressions::SUNGLASS_LR);
    expr.attr("SUNGLASS_UPR") = py::str(EyeExpressions::SUNGLASS_UPR);
    expr.attr("THINK") = py::str(EyeExpressions::THINK);
    expr.attr("THREATENED_L") = py::str(EyeExpressions::THREATENED_L);
    expr.attr("THREATENED_R") = py::str(EyeExpressions::THREATENED_R);
    expr.attr("THRILLED") = py::str(EyeExpressions::THRILLED);
    expr.attr("THROWN") = py::str(EyeExpressions::THROWN);
    expr.attr("TIRED") = py::str(EyeExpressions::TIRED);
    expr.attr("TROUBLED") = py::str(EyeExpressions::TROUBLED);
    expr.attr("TURNED_ON") = py::str(EyeExpressions::TURNED_ON);
    expr.attr("UNCOMFORTABLE") = py::str(EyeExpressions::UNCOMFORTABLE);
    expr.attr("UNHAPPY") = py::str(EyeExpressions::UNHAPPY);
    expr.attr("UPSET") = py::str(EyeExpressions::UPSET);
    expr.attr("VR") = py::str(EyeExpressions::VR);
    expr.attr("WAKE_WORD") = py::str(EyeExpressions::WAKE_WORD);
    expr.attr("WORKOUT") = py::str(EyeExpressions::WORKOUT);
    expr.attr("ZOOM_IN") = py::str(EyeExpressions::ZOOM_IN);

    // -------------------------------
    // EyeControl API
    // -------------------------------
    m.def(
        "init",
        [](ColorCode eye_color, ColorCode bg_color) {
            g_callbacks_enabled.store(true, std::memory_order_relaxed);
            return EyeControl::init(eye_color, bg_color);
        },
        py::arg("eye_color"), py::arg("bg_color"),
        "Initialize eye control.\n"
        "Returns 0 on success."
    );

    m.def("abort", &EyeControl::Abort, "Terminate active animation.");
    m.def("is_active", &EyeControl::isActive, "Return True if initialized/active.");
    m.def("is_animating", &EyeControl::isAnimating, "Return True if animation is running.");

    m.def("set_eyes", &EyeControl::setEyes,
          py::arg("shape"), py::arg("iris_color"), py::arg("bg_color"),
          "Set both eyes by shape and colors.");

    m.def("set_iris", py::overload_cast<IrisShape, ColorCode, EyeSide>(&EyeControl::setIris),
          py::arg("shape"), py::arg("color"), py::arg("side"),
          "Set iris shape and color.");



    m.def(
        "get_iris_position",
        [](EyeSide side) {
            int16_t x = 0, y = 0;
            int8_t rc = EyeControl::getIrisPosition(side, x, y);
            return py::make_tuple(rc, x, y);
        },
        py::arg("side"),
        "Get iris position. Returns (result, x, y)."
    );

    m.def("set_background", py::overload_cast<Color>(&EyeControl::setBackground),
          py::arg("color"),
          "Set background color.");
    // VContent overloads (uses doly_vcontent.VContent)
    m.def(
        "set_iris_image",
        [](VContent &eye_image, EyeSide side) {
            return EyeControl::setIris(&eye_image, side);
        },
        py::arg("eye_image"), py::arg("side"),
        "Set iris using a VContent image."
    );

    m.def(
        "set_lid_image",
        [](VContent &lid_image, bool is_top, EyeSide side) {
            return EyeControl::setLid(&lid_image, is_top, side);
        },
        py::arg("lid_image"), py::arg("is_top"), py::arg("side"),
        "Set eyelid image using a VContent image."
    );

    m.def(
        "set_background_image",
        [](VContent &bg_image, EyeSide side) {
            return EyeControl::setBackground(&bg_image, side);
        },
        py::arg("bg_image"), py::arg("side"),
        "Set background image using a VContent image."
    );

    m.def(
        "set_animation",
        [](uint16_t id, const std::string &name) {
            // API expects std::string_view; keep a stable std::string during the call
            return EyeControl::setAnimation(id, std::string_view(name));
        },
        py::arg("id"), py::arg("name"),
        "Start an animation by id and name."
    );

    m.def(
        "set_position",
        &EyeControl::setPosition,
        py::arg("side"),
        py::arg("centerX"), py::arg("centerY"),
        py::arg("scaleX") = 1.0f, py::arg("scaleY") = 1.0f,
        py::arg("lid_top_end") = 0, py::arg("lid_bot_start") = 240,
        "Set eye position and scale."
    );

    m.def(
        "dispose",
        []() {
            clear_listeners_impl();
            // EyeControl has no dispose() in header; keep this for consistent API.
        },
        "Dispose wrapper callbacks/listeners (EyeControl has no dispose function)."
    );

    m.def("get_version", &EyeControl::getVersion, "Get version (0.XYZ).");
}
