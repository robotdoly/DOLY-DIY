#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

#include "VContent.h"

namespace py = pybind11;

PYBIND11_MODULE(doly_vcontent, m)
{
    m.doc() = "Bindings for VContent (pybind11)";

    py::class_<VContent>(m, "VContent")
        .def(py::init<>(), "Create an empty VContent.")

        .def("is_ready", &VContent::isReady, "Return True if the content has been loaded successfully.")

        // Public fields (kept simple; these are already public in C++)
        .def_readonly("active_frame_id", &VContent::active_frame_id, "Selected frame on load.")
        .def_readonly("ft", &VContent::ft, "Total frames in sequence.")
        .def_readonly("width", &VContent::width, "Frame width.")
        .def_readonly("height", &VContent::height, "Frame height.")
        .def_readonly("path", &VContent::path, "Source path.")
        .def_readonly("alpha", &VContent::alpha, "True if content has alpha channel.")
        .def_readonly("color12Bit", &VContent::color12Bit, "True if content is 12-bit color (or more).")
        .def_readonly("ratio", &VContent::ratio, "Frame ratio divider.")
        .def_readonly("loop", &VContent::loop, "Loop count (0 = forever).")

        // Frames accessors
        .def_property_readonly(
            "frame_count",
            [](const VContent &c)
            { return static_cast<std::size_t>(c.frames.size()); },
            "Number of frames loaded.")
        .def(
            "get_frame_bytes",
            [](const VContent &c, std::size_t index) -> py::bytes
            {
                if (index >= c.frames.size())
                    throw py::index_error("frame index out of range");
                const auto &buf = c.frames[index];
                if (buf.empty())
                    return py::bytes();
                return py::bytes(reinterpret_cast<const char *>(buf.data()), static_cast<py::ssize_t>(buf.size()));
            },
            py::arg("index") = 0,
            "Get a frame as bytes (raw RGB/RGBA buffer as stored by the library).")

        // Expose frames as-is (may be heavy, but useful for debugging)
        .def_readwrite("frames", &VContent::frames, "Raw frames as list of byte arrays (debug/advanced).")

        // Static loader
        .def_static(
            "get_image",
            [](const std::string &path, bool isRGBA, bool set12Bit)
            {
                return VContent::getImage(path, isRGBA, set12Bit);
            },
            py::arg("path"), py::arg("isRGBA"), py::arg("set12Bit"),
            "Load a PNG from path and return a VContent.\n"
            "isRGBA: True if loaded image has alpha.\n"
            "set12Bit: convert to 12-bit color depth.");
}
