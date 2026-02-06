#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Color.h"

namespace py = pybind11;

PYBIND11_MODULE(doly_color, m) {
    m.doc() = "Common color types (ColorCode, Color) used by multiple Doly libraries.";

    py::enum_<ColorCode>(m, "ColorCode")
        .value("Black", ColorCode::BLACK)
        .value("White", ColorCode::WHITE)
        .value("Gray", ColorCode::GRAY)
        .value("Salmon", ColorCode::SALMON)
        .value("Red", ColorCode::RED)
        .value("DarkRed", ColorCode::DARK_RED)
        .value("Pink", ColorCode::PINK)
        .value("Orange", ColorCode::ORANGE)
        .value("Gold", ColorCode::GOLD)
        .value("Yellow", ColorCode::YELLOW)
        .value("Purple", ColorCode::PURPLE)
        .value("Magenta", ColorCode::MAGENTA)
        .value("Lime", ColorCode::LIME)
        .value("Green", ColorCode::GREEN)
        .value("DarkGreen", ColorCode::DARK_GREEN)
        .value("Cyan", ColorCode::CYAN)
        .value("SkyBlue", ColorCode::SKY_BLUE)
        .value("Blue", ColorCode::BLUE)
        .value("DarkBlue", ColorCode::DARK_BLUE)
        .value("Brown", ColorCode::BROWN);

    py::class_<Color>(m, "Color")
        .def(py::init<>())
        .def_readwrite("r", &Color::r)
        .def_readwrite("g", &Color::g)
        .def_readwrite("b", &Color::b)
        .def("toString", &Color::toString, "Return string representation.")
        .def_static("get_color",
                    py::overload_cast<uint8_t, uint8_t, uint8_t>(&Color::getColor),
                    py::arg("r"), py::arg("g"), py::arg("b"),
                    "Create a Color from r,g,b.")
        .def_static("hex_to_rgb", &Color::hexToRgb, py::arg("hex"),
                    "Convert hex string to Color.")
        .def_static("from_code",
                    py::overload_cast<ColorCode>(&Color::getColor),
                    py::arg("code"),
                    "Get a Color from ColorCode.")
        .def_static("get_led_color", &Color::getLEDColor, py::arg("code"),
                    "Get closest color tone for LEDs.")
        .def_static("get_color_name", &Color::getColorName, py::arg("code"),
                    "Get color name from ColorCode.")
        .def("__repr__", [](const Color &c) {
            return "Color(r=" + std::to_string(c.r) + ", g=" + std::to_string(c.g) + ", b=" + std::to_string(c.b) + ")";
        });
}
