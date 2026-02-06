#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <opencv2/opencv.hpp>
#include <cstring>   // std::memcpy

#include "lccv.hpp"
#include "libcamera_app_options.hpp"

namespace py = pybind11;

// Convert cv::Mat -> numpy array (COPY for safety; avoids libcamera buffer lifetime issues)
static py::array mat_to_numpy_copy(const cv::Mat& mat)
{
    if (mat.empty() || mat.cols <= 0 || mat.rows <= 0)
        return py::array();

    cv::Mat src = mat;
    if (!src.isContinuous())
        src = src.clone();

    const int h = src.rows;
    const int w = src.cols;
    const int ch = src.channels();

    // Only handle 8-bit images cleanly (most camera outputs here are CV_8U)
    if (src.depth() != CV_8U)
    {
        cv::Mat tmp;
        src.convertTo(tmp, CV_8U);
        src = tmp;
    }

    if (ch == 1)
    {
        auto out = py::array_t<uint8_t>({h, w});
        std::memcpy(out.mutable_data(), src.data, (size_t)h * (size_t)w);
        return out;
    }
    else if (ch == 3)
    {
        auto out = py::array_t<uint8_t>({h, w, 3});
        std::memcpy(out.mutable_data(), src.data, (size_t)h * (size_t)w * 3);
        return out;
    }
    else if (ch == 4)
    {
        auto out = py::array_t<uint8_t>({h, w, 4});
        std::memcpy(out.mutable_data(), src.data, (size_t)h * (size_t)w * 4);
        return out;
    }

    // Unsupported channel count
    return py::array();
}

PYBIND11_MODULE(doly_camera, m)
{
    m.doc() = "Doly camera module (LCCV/libcamera) - frames returned as NumPy arrays.";

    // ----- Enums (optional but useful) -----
    py::enum_<Exposure_Modes>(m, "ExposureModes")
        .value("EXPOSURE_NORMAL", EXPOSURE_NORMAL)
        .value("EXPOSURE_SHORT", EXPOSURE_SHORT)
        .value("EXPOSURE_CUSTOM", EXPOSURE_CUSTOM)
        .export_values();

    py::enum_<Metering_Modes>(m, "MeteringModes")
        .value("METERING_CENTRE", METERING_CENTRE)
        .value("METERING_SPOT", METERING_SPOT)
        .value("METERING_MATRIX", METERING_MATRIX)
        .value("METERING_CUSTOM", METERING_CUSTOM)
        .export_values();

    py::enum_<WhiteBalance_Modes>(m, "WhiteBalanceModes")
        .value("WB_AUTO", WB_AUTO)
        .value("WB_NORMAL", WB_NORMAL)
        .value("WB_INCANDESCENT", WB_INCANDESCENT)
        .value("WB_TUNGSTEN", WB_TUNGSTEN)
        .value("WB_FLUORESCENT", WB_FLUORESCENT)
        .value("WB_INDOOR", WB_INDOOR)
        .value("WB_DAYLIGHT", WB_DAYLIGHT)
        .value("WB_CLOUDY", WB_CLOUDY)
        .value("WB_CUSTOM", WB_CUSTOM)
        .export_values();

    // ----- Options -----
    py::class_<Options>(m, "Options")
        .def(py::init<>())

        .def_readwrite("help", &Options::help)
        .def_readwrite("version", &Options::version)
        .def_readwrite("list_cameras", &Options::list_cameras)
        .def_readwrite("verbose", &Options::verbose)

        .def_readwrite("timeout", &Options::timeout)
        .def_readwrite("photo_width", &Options::photo_width)
        .def_readwrite("photo_height", &Options::photo_height)
        .def_readwrite("video_width", &Options::video_width)
        .def_readwrite("video_height", &Options::video_height)

        .def_readwrite("roi_x", &Options::roi_x)
        .def_readwrite("roi_y", &Options::roi_y)
        .def_readwrite("roi_width", &Options::roi_width)
        .def_readwrite("roi_height", &Options::roi_height)

        .def_readwrite("shutter", &Options::shutter)
        .def_readwrite("gain", &Options::gain)
        .def_readwrite("ev", &Options::ev)

        .def_readwrite("awb_gain_r", &Options::awb_gain_r)
        .def_readwrite("awb_gain_b", &Options::awb_gain_b)

        .def_readwrite("brightness", &Options::brightness)
        .def_readwrite("contrast", &Options::contrast)
        .def_readwrite("saturation", &Options::saturation)
        .def_readwrite("sharpness", &Options::sharpness)

        .def_readwrite("framerate", &Options::framerate)
        .def_readwrite("denoise", &Options::denoise)
        .def_readwrite("info_text", &Options::info_text)
        .def_readwrite("camera", &Options::camera)

        // helper methods
        .def("set_metering", &Options::setMetering, py::arg("mode"))
        .def("set_white_balance", &Options::setWhiteBalance, py::arg("mode"))
        .def("set_exposure_mode", &Options::setExposureMode, py::arg("mode"))
        .def("get_exposure_mode", &Options::getExposureMode)
        .def("get_metering_mode", &Options::getMeteringMode)
        .def("get_white_balance", &Options::getWhiteBalance);

    // ----- PiCamera -----
    py::class_<PiCamera>(m, "PiCamera")
        .def(py::init<>())

        // Expose the Options owned by PiCamera
        .def_property_readonly(
            "options",
            [](PiCamera& self) -> Options& {
                return *self.options;
            },
            py::return_value_policy::reference_internal,
            "Camera options (owned by PiCamera)."
        )

        // Photo mode
        .def("start_photo", [](PiCamera& self) {
            py::gil_scoped_release r;
            return self.startPhoto();
        })
        .def("capture_photo", [](PiCamera& self) -> py::object {
            cv::Mat frame;
            bool ok = false;
            {
                py::gil_scoped_release r;
                ok = self.capturePhoto(frame);
            }
            if (!ok || frame.empty())
                return py::none();

            return mat_to_numpy_copy(frame); // <-- NO py::cast
        })
        .def("stop_photo", [](PiCamera& self) {
            py::gil_scoped_release r;
            return self.stopPhoto();
        })

        // Video mode
        .def("start_video", [](PiCamera& self) {
            py::gil_scoped_release r;
            return self.startVideo();
        })
        .def("get_video_frame", [](PiCamera& self, unsigned int timeout_ms) -> py::object {
            cv::Mat frame;
            bool ok = false;
            {
                py::gil_scoped_release r;
                ok = self.getVideoFrame(frame, timeout_ms);
            }
            if (!ok || frame.empty())
                return py::none();

            return mat_to_numpy_copy(frame); // <-- NO py::cast
        }, py::arg("timeout_ms") = 1500)
        .def("stop_video", [](PiCamera& self) {
            py::gil_scoped_release r;
            self.stopVideo();
        })

        // Apply zoom (ROI) after updating options->roi_*
        .def("apply_zoom_options", [](PiCamera& self) {
            py::gil_scoped_release r;
            self.ApplyZoomOptions();
        })

        // Custom controls
        .def("set_exposure", [](PiCamera& self, float value) {
            py::gil_scoped_release r;
            self.SetExposure(value);
        }, py::arg("value"))
        .def("set_awb_enable", [](PiCamera& self, bool enable) {
            py::gil_scoped_release r;
            self.SetAwbEnable(enable);
        }, py::arg("enable"));
}
