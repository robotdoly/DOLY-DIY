#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "TtsControl.h"

namespace py = pybind11;


PYBIND11_MODULE(doly_tts, m)
{
    m.doc() = "Bindings for TtsControl (pybind11)";

    py::enum_<VoiceModel>(m, "VoiceModel")
        .value("Model1", VoiceModel::MODEL_1)
        .value("Model2", VoiceModel::MODEL_2)
        .value("Model3", VoiceModel::MODEL_3);

    m.def(
        "init",
        [](VoiceModel model, const std::string &output_path) {                      
            return TtsControl::init(model, output_path);
        },
        py::arg("model"), py::arg("output_path") = "",
        "Initialize TTS system.\n"
        "Returns 0 on success, 1 if already initialized." 
    );
    
    m.def(
        "dispose",
        []() {
            return TtsControl::dispose();
        },
        "Dispose TTS system (also clears callbacks first)."
    );

    m.def(
        "produce",
        [](const std::string &text) {            
            return TtsControl::produce(text);
        },
        py::arg("text"),
        "Produce a sound file from text.\n"
        "Returns 0 on success, -1 if not active, -2 on processing error."
    );

    m.def("get_version", &TtsControl::getVersion, "Get TTS version (0.XYZ)." );
   
}
