#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>

#include "LcdControl.h"

namespace py = pybind11;

// Helper: accept any Python buffer (bytes/bytearray/memoryview/numpy uint8) as uint8_t*
static std::pair<uint8_t*, size_t> get_u8_buffer(py::buffer b) {
    py::buffer_info info = b.request();
    if (info.itemsize != 1) {
        throw std::runtime_error("Expected a uint8/bytes buffer (itemsize=1). ");
    }
    // Require contiguous
    if (!info.strides.empty()) {
        for (ssize_t i = 0; i < static_cast<ssize_t>(info.strides.size()); ++i) {
            // A simple contiguous check: last stride should be itemsize, others should be product of later dims.
            // Many buffers will pass with standard contiguous layout.
            if (info.strides[i] == 0) {
                throw std::runtime_error("Buffer has zero stride (not supported).");
            }
        }
    }
    return {static_cast<uint8_t*>(info.ptr), static_cast<size_t>(info.size)};
}

PYBIND11_MODULE(doly_lcd, m) {
    m.doc() = "Bindings for LcdControl (pybind11)";

    py::enum_<LcdColorDepth>(m, "LcdColorDepth")
        .value("L12BIT", LcdColorDepth::L12BIT)
        .value("L18BIT", LcdColorDepth::L18BIT);

    py::enum_<LcdSide>(m, "LcdSide")
        .value("Left", LcdSide::LEFT)
        .value("Right", LcdSide::RIGHT);

    // Note: LcdData includes a raw pointer. We expose a safer Python API for write_lcd.

    m.def(
        "init",
        [](LcdColorDepth depth) {           
            return LcdControl::init(depth);
        },
        py::arg("depth") = LcdColorDepth::L12BIT,
        "Initialize LCD control.\n"
        "Returns 0 on success, 1 if already initialized, negative on error."
    );

    m.def(
        "dispose",
        []() {
            return LcdControl::dispose();
        },
        "Dispose LCD control (also clears callbacks first)."
    );

    m.def("is_active", &LcdControl::isActive, "Return True if initialized/active.");

    m.def(
        "lcd_color_fill",
        &LcdControl::LcdColorFill,
        py::arg("side"), py::arg("r"), py::arg("g"), py::arg("b"),
        "Fill a LCD side with an RGB color."
    );

    m.def(
        "write_lcd",
        [](LcdSide side, py::buffer buffer) {
            auto [ptr, n] = get_u8_buffer(buffer);
            // Optional safety: ensure buffer matches expected size (when available)
            int expected = LcdControl::getBufferSize();
            if (expected > 0 && static_cast<int>(n) < expected) {
                throw std::runtime_error("Buffer is smaller than get_buffer_size().");
            }

            LcdData data;
            data.side = side;
            data.buffer = ptr;
            return LcdControl::writeLcd(&data);
        },
        py::arg("side"), py::arg("buffer"),
        "Write a frame buffer to the LCD.\n"
        "buffer can be bytes/bytearray/memoryview/numpy uint8.\n"
        "Returns 0 on success, negative on error."
    );

    m.def("get_buffer_size", &LcdControl::getBufferSize,
          "Return LCD frame buffer size in bytes for the current color depth.");

    m.def("get_color_depth", &LcdControl::getColorDepth,
          "Return current LCD color depth.");

    m.def(
        "set_brightness",
        &LcdControl::setBrightness,
        py::arg("value"),
        "Set brightness (min=0, max=10). Returns 0 on success."
    );

    m.def(
        "to_lcd_buffer",
        [](py::buffer input, bool input_rgba) {
            // Allocate output of the correct size.
            int out_size = LcdControl::getBufferSize();
            if (out_size <= 0) {
                throw std::runtime_error("Invalid buffer size. Call init() first.");
            }

            auto [in_ptr, in_n] = get_u8_buffer(input);

            // Best-effort input size validation
            LcdColorDepth depth = LcdControl::getColorDepth();
            int out_bpp = (depth == LcdColorDepth::L18BIT) ? 3 : 2; // typical
            int pixel_count = out_size / out_bpp;
            size_t expected_in = static_cast<size_t>(pixel_count) * (input_rgba ? 4u : 3u);
            if (in_n < expected_in) {
                throw std::runtime_error("Input buffer is smaller than expected for the current LCD settings.");
            }

            std::vector<uint8_t> out(static_cast<size_t>(out_size));
            LcdControl::toLcdBuffer(out.data(), in_ptr, input_rgba);
            return py::bytes(reinterpret_cast<const char*>(out.data()), out.size());
        },
        py::arg("input"), py::arg("input_rgba") = false,
        "Convert 24-bit (RGB) or 32-bit (RGBA) image bytes into the LCD frame buffer format.\n"
        "Returns bytes of length get_buffer_size()."
    );

    m.def("get_version", &LcdControl::getVersion, "Get version (0.XYZ)."
    );
}
