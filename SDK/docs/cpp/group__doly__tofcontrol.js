var group__doly__tofcontrol =
[
    [ "TofControl.h", "_tof_control_8h.html", null ],
    [ "TofEvent.h", "_tof_event_8h.html", null ],
    [ "TofGesture", "struct_tof_gesture.html", [
      [ "range_mm", "struct_tof_gesture.html#aff705a2cc2ea8e2e65dbaf2837655584", null ],
      [ "type", "struct_tof_gesture.html#a5ba20ebfb3cd018caf939764d125ef37", null ]
    ] ],
    [ "TofData", "struct_tof_data.html", [
      [ "error", "struct_tof_data.html#a712fafc25d65649e0ab036978a5db197", null ],
      [ "range_mm", "struct_tof_data.html#a6906f3bb82f7279fc24b57b6b6df95ec", null ],
      [ "side", "struct_tof_data.html#a7926c0acad87bd6a53293e771d8e8524", null ],
      [ "update_ms", "struct_tof_data.html#ab01faedfd5324bb7d8365d6119dfc858", null ]
    ] ],
    [ "TofEventListener", "class_tof_event_listener.html", [
      [ "~TofEventListener", "class_tof_event_listener.html#a43638595a45c802a1f6cf70ddde355f2", null ],
      [ "onProximityGesture", "class_tof_event_listener.html#ad716fa2d449c8b568159cbd102f61dfb", null ],
      [ "onProximityThreshold", "class_tof_event_listener.html#a0a9c0fe2b655cb9de07a653ac44e6d14", null ]
    ] ],
    [ "TofError", "group__doly__tofcontrol.html#ga20bb9d362a75c16307b8cd2582473b03", [
      [ "TofError::NO_ERROR", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03ad306b6fdee05fe87455110ddf6501e6c", null ],
      [ "TofError::VCSEL_Continuity_Test", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03a06ac35724b7223db539b0bf49b4a6592", null ],
      [ "TofError::VCSEL_Watchdog_Test", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03a420e015deab547d2325ca691e5cd5cf7", null ],
      [ "TofError::VCSEL_Watchdog", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03afdb4c503ab9fa8ba25c4014e5f76e1da", null ],
      [ "TofError::PLL1_Lock", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03a97e5eb28533deed86ae486ad32fe5622", null ],
      [ "TofError::PLL2_Lock", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03a22c55f2fe4f5f2f0a87932205d3c0273", null ],
      [ "TofError::Early_Convergence_Estimate", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03aed2736ba86ff4b1e81dd09beedf82f5d", null ],
      [ "TofError::Max_Convergence", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03ad104723acfcd8106f6d4f1cb73b191c1", null ],
      [ "TofError::No_Target_Ignore", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03aecc17e7ea915c57e583ca9dd12eeaca0", null ],
      [ "TofError::Max_Signal_To_Noise_Ratio", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03a3aaee50a1f3d2a03eb30fe3246f60b23", null ],
      [ "TofError::Raw_Ranging_Algo_Underflow", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03ad01e5555f89734a90969b9fb09570711", null ],
      [ "TofError::Raw_Ranging_Algo_Overflow", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03afed1a2e8022ed2a626ef4c408fc17a23", null ],
      [ "TofError::Ranging_Algo_Underflow", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03a8408ce64a4aeda8d51a644b3febcaff4", null ],
      [ "TofError::Ranging_Algo_Overflow", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03a30c4dc55eff905564cc3f736f298b5bb", null ],
      [ "TofError::Filtered_by_post_processing", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03af5c2c4957fb14617bb70a4a6cccee280", null ],
      [ "TofError::DataNotReady", "group__doly__tofcontrol.html#gga20bb9d362a75c16307b8cd2582473b03aa890f97663b40ad7366c90a76e6bf979", null ]
    ] ],
    [ "TofGestureType", "group__doly__tofcontrol.html#ga194e4bd34dc20440f3baab83f96bd2c3", [
      [ "TofGestureType::UNDEFINED", "group__doly__tofcontrol.html#gga194e4bd34dc20440f3baab83f96bd2c3a0db45d2a4141101bdfe48e3314cfbca3", null ],
      [ "TofGestureType::OBJECT_COMING", "group__doly__tofcontrol.html#gga194e4bd34dc20440f3baab83f96bd2c3ab0ac84da304a71669b02f5ce4df333a8", null ],
      [ "TofGestureType::OBJECT_GOING", "group__doly__tofcontrol.html#gga194e4bd34dc20440f3baab83f96bd2c3a3ed53d9221a0041de9085b369d312117", null ],
      [ "TofGestureType::SCRUBING", "group__doly__tofcontrol.html#gga194e4bd34dc20440f3baab83f96bd2c3aabde72ddbf3c34ee4146404837ce0595", null ],
      [ "TofGestureType::TO_LEFT", "group__doly__tofcontrol.html#gga194e4bd34dc20440f3baab83f96bd2c3aa6e076210111b883abc496eef27f1882", null ],
      [ "TofGestureType::TO_RIGHT", "group__doly__tofcontrol.html#gga194e4bd34dc20440f3baab83f96bd2c3a7e5c7d2d0fdb4ebb69b6802752b389c7", null ]
    ] ],
    [ "TofSide", "group__doly__tofcontrol.html#ga8cea48fefeb2f36902e57749bf5514d5", [
      [ "TofSide::LEFT", "group__doly__tofcontrol.html#gga8cea48fefeb2f36902e57749bf5514d5a684d325a7303f52e64011467ff5c5758", null ],
      [ "TofSide::RIGHT", "group__doly__tofcontrol.html#gga8cea48fefeb2f36902e57749bf5514d5a21507b40c80068eda19865706fdc2403", null ]
    ] ]
];