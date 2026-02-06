#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <atomic>
#include <cstdint>

#include "IoControl.h"

namespace py = pybind11;

PYBIND11_MODULE(doly_io, m) {
    // Enums / structs
    py::enum_<GpioState>(m, "GpioState")
        .value("Low", GpioState::LOW)
        .value("High", GpioState::HIGH);

    m.doc() = "Bindings for IoControl (pybind11)";

    m.def(
        "write_pin",
        [](uint16_t id, uint8_t io_pin, GpioState state) -> int8_t {           
            return IoControl::writePin(id, io_pin, state);
        },
        py::arg("id"),
        py::arg("io_pin"),
        py::arg("state"),
        "Write a GPIO IO pin.\n"
        "Returns 0 on success.\n"
        "-1: invalid io_pin (must be 0..5)\n"
        "-2: gpio write error\n"
        "id is for internal purposes and can be any value.\n"
        "state: GPIO state (0=LOW, 1=HIGH)."
    );

    m.def(
        "read_pin",
        [](uint16_t id, uint8_t io_pin) -> GpioState {           
            return IoControl::readPin(id, io_pin);
        },
        py::arg("id"),
        py::arg("io_pin"),
        "Read a GPIO IO pin.\n"
        "Returns GPIO state (0=LOW, 1=HIGH).\n"
        "io_pin must be 0..5. id is for internal purposes and can be any value."
    );

    m.def("get_version", &IoControl::getVersion, "Get version (0.XYZ)."
    );
   
}
