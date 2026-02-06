#include <pybind11/pybind11.h>
#include <pybind11/stl.h>          // for std::tuple conversions if needed
#include <pybind11/operators.h>

#include "Helper.h"

namespace py = pybind11;

PYBIND11_MODULE(doly_helper, m)
{
    m.doc() = "Python bindings for Helper library (pybind11)";

    // ---- Structs ----
    py::class_<Position>(m, "Position")
        .def(py::init<>())
        .def_readwrite("head", &Position::head)
        .def_readwrite("x", &Position::x)
        .def_readwrite("y", &Position::y);

    py::class_<Position2F>(m, "Position2F")
        .def(py::init<>())
        .def_readwrite("x", &Position2F::x)
        .def_readwrite("y", &Position2F::y);

    // ---- Functions ----
    // Same signature: returns int8 status code
    m.def("read_settings", &Helper::readSettings,
          "Read default settings from file. Returns int status code.");

    // In C++ it fills references; in Python we return (status, gx, gy, gz, ax, ay, az)
    m.def("get_imu_offsets",
          []() {
              int16_t gx = 0, gy = 0, gz = 0, ax = 0, ay = 0, az = 0;
              int8_t res = Helper::getImuOffsets(gx, gy, gz, ax, ay, az);
              return py::make_tuple((int)res, gx, gy, gz, ax, ay, az);
          },
          "Get IMU offsets. Returns (status, gx, gy, gz, ax, ay, az).");

    m.def("stop_doly_service", &Helper::stopDolyService,
          "Stop the Doly service. Returns int status code.");
}
