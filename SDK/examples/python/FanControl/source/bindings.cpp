#include <pybind11/pybind11.h>

#include "FanControl.h"

namespace py = pybind11;

PYBIND11_MODULE(doly_fan, m) {
    m.doc() = "Bindings for FanControl (pybind11)";

    m.def(
        "init",
        [](bool auto_control) {           
            return FanControl::init(auto_control);
        },
        py::arg("auto_control"),
        "Initialize Temperature Control.\n"
        "auto_control: if True, automatic fan control manages speed based on temperature."
    );   

    m.def(
        "dispose",
        []() {
            return FanControl::dispose();
        },
        "Dispose Temperature Control (also clears callbacks first)."
    );

    m.def(
        "set_fan_speed",
        [](int percentage) {
            if (percentage < 0) percentage = 0;
            if (percentage > 100) percentage = 100;
            return FanControl::setFanSpeed(static_cast<uint8_t>(percentage));
        },
        py::arg("percentage"),
        "Set fan speed (0-100 percentage)."
    );

    m.def("is_active", &FanControl::isActive, "Return True if initialized/active.");
    m.def("get_version", &FanControl::getVersion, "Get version (0.XYZ).");
}
