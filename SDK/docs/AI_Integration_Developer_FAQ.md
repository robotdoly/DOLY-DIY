# Using Doly with OpenAI, Gemini, and Other AI Models

## Developer FAQ

Doly can be used as a programmable robot platform for custom AI applications. Developers can stop the standard Doly service, run their own application directly on Doly's Raspberry Pi, use the Doly C++ or Python SDK for robot control, and use standard Linux interfaces such as ALSA for audio.

The AI portion of the application can then be implemented by the developer using the AI model or service that fits the project, such as OpenAI, Google Gemini, or another service.

> This FAQ describes custom developer use. It does not mean that every AI provider has an official Doly integration.

Useful starting points:

- [Doly Developer page](https://doly.ai/developer/)
- [DOLY-DIY GitHub repository](https://github.com/robotdoly/DOLY-DIY)
- [C++ SDK documentation](https://robotdoly.github.io/DOLY-DIY/cpp/)
- [Python SDK documentation](https://robotdoly.github.io/DOLY-DIY/python/)

---

### Can I use Doly with OpenAI, Gemini, or another AI model?

Yes. You can run your own C++ or Python application directly on Doly and build the AI integration inside that application.

Doly's SDK can handle robot functions such as movement, camera access, sensors, eyes/displays, LEDs, sound, and other hardware, while your application handles communication with the AI model or service you choose.

The AI provider does not need an official Doly integration. Your application can act as the integration layer between Doly and the provider's API.

See the [Doly Developer page](https://doly.ai/developer/) and [DOLY-DIY SDK](https://github.com/robotdoly/DOLY-DIY/tree/main/SDK).

---

### Do I have to use Doly's default personality or software?

No. You can stop Doly's standard service and run your own application instead.

The official SDK examples stop the Doly service before directly using Doly libraries because running multiple instances of the same hardware-control libraries can cause conflicts. For example, see the [BatteryControl C++ example](https://github.com/robotdoly/DOLY-DIY/blob/main/SDK/examples/cpp/BatteryControl/main.cpp).

You can also configure your own application as a normal Linux service so that it starts automatically when Doly boots.

---

### Does my custom AI application need the Doly mobile app or other service? Can it operate entirely offline?

No for local robot control. A custom application can control Doly locally without depending on the Doly mobile app or other services.

Doly can operate fully offline, if your own application uses an external cloud service, such as an online AI model, that part of the application will require whatever internet connection the selected provider requires. Internet access can also be useful for optional Doly functions such as software updates and online utilities, like weather forecats, but it is not required for a custom local-control application.

See the [Doly Support FAQ](https://doly.ai/support/) and [Doly Developer page](https://doly.ai/developer/).

---

### Can a C++ or Python application continuously access Doly's 8 MP camera?

Yes. The Doly C++ and Python camera interfaces provide direct access to captured frames, including continuous video-frame access. A custom application can process those frames locally or send selected image/video data through its own networking or AI pipeline without using the official Doly application.

The published Doly camera examples demonstrate both still-image capture and continuous video-frame capture:

- [C++ camera example](https://github.com/robotdoly/DOLY-DIY/blob/main/SDK/examples/cpp/Camera/main.cpp)
- [Python camera example](https://github.com/robotdoly/DOLY-DIY/blob/main/SDK/examples/python/Camera/example.py)

Doly uses the **Sony IMX219** 8 MP image sensor, the same sensor family used by the Raspberry Pi Camera Module V2.

Actual usable resolution and frame rate also depend on processing load, image conversion, encoding, networking, and the Raspberry Pi CM4 configuration. Developers should test the selected mode under their real workload rather than assume the sensor's maximum mode is the best streaming setting.

For sensor-mode details, see the [Raspberry Pi Camera V2 sensor-mode documentation](https://picamera.readthedocs.io/en/release-1.13/fov.html#sensor-modes).

---

### Can I access the onboard microphones directly?

Yes. With the Doly service stopped, the onboard microphones can be accessed through ALSA or an ALSA-based library.

A custom application can continuously capture and process raw microphone audio. This allows developers to use their own speech recognition, wake-word, voice-processing, recording, or AI audio pipeline.

---

### Can microphone capture run while I control other parts of Doly?

Yes.

Doly libraries/APIs can operate on different threads. A custom application can capture microphone audio while also using Doly APIs for functions such as the camera, drive system, arms, eyes/displays, LEDs, and sensors.

As with any multithreaded application, the developer is responsible for synchronization and for avoiding conflicting access to the same hardware resource.

---

### Can Doly record from the microphones while playing audio through the speakers?

Yes. Full-duplex audio operation is possible.

A custom application can capture microphone audio while simultaneously playing arbitrary audio through Doly's speakers.

For conversational applications, developers may also need acoustic echo cancellation and noise processing so that speaker output is not interpreted as new microphone input.

---

### Does Doly have hardware Acoustic Echo Cancellation (AEC)?

No. Doly uses the **Texas Instruments TLV320AIC3110** audio codec. It provides audio ADC/DAC, microphone input functions, speaker amplification, filtering, automatic gain control, and other audio-processing features, but it does not provide a dedicated on-chip Acoustic Echo Cancellation function.

Doly's standard software uses third-party software libraries for acoustic echo cancellation and noise processing, but those libraries are not currently exposed through the Doly SDK.

For a custom application, developers can select their own software AEC and noise-processing solution.

See the [Texas Instruments TLV320AIC3110 product documentation](https://www.ti.com/product/TLV320AIC3110) and Doly's [electronics schematics](https://github.com/robotdoly/DOLY-DIY/tree/main/Electronics/Schematic).

---

### Can I use my own speech recognition, TTS, or voice system?

Yes.

After disabling the standard Doly service, you can use your own audio, speech-recognition, wake-word, text-to-speech, or voice-processing solution.

You are not required to use Doly's built-in TTS system for a custom application.

---

### Can different Doly functions run at the same time?

Yes.

The libraries/APIs can be used on different threads within a custom application, allowing an AI program to listen, see, process sensor information, and control robot functions concurrently.

Developers should still coordinate access when multiple tasks use the same device or library resource at the same time.

---

### Can I continuously run my own application on Doly?

Yes.

Doly runs Raspberry Pi OS / Debian Linux on its Raspberry Pi CM4. You can disable Doly's standard service and configure your own application or Linux service to start automatically at boot.

---

### Can Doly charge while my custom software is running?

Yes.

Charging is controlled at the hardware level and does not depend on the standard Doly application running.

Doly can remain physically docked and charge normally while the standard Doly service is stopped and your custom C++ or Python application continues running.

---

### Can custom software read Doly's battery level, charging state, and docking status?

**Battery capacity:** Yes. Battery capacity can be read directly through the Doly SDK. The C++ [`BatteryControl` example](https://github.com/robotdoly/DOLY-DIY/blob/main/SDK/examples/cpp/BatteryControl/main.cpp) demonstrates reading the current percentage with `BatteryControl::getCapacity()`.

**Charging state and docking status:** The current `BatteryControl` library does not expose these as direct SDK status fields.

Doly uses the **Texas Instruments INA219** bidirectional current/power monitor. Doly's standard software determines charging state from the direction of battery current using averaged INA219 readings. In Doly's hardware/current convention, a positive average current indicates that the battery is charging, and Doly treats that as being on the charging dock.

A custom application can implement similar logic by reading the INA219 directly. This is an inferred charging/dock state rather than a dedicated docking-state field in the current `BatteryControl` API.

The INA219 supports bidirectional current measurement over I2C. See the [Texas Instruments INA219 documentation](https://www.ti.com/product/INA219) and the [Doly Community battery discussion](https://community.doly.ai/public/d/34-doly-battery-level-via-sdk/3).

---

### Can I inspect Doly's electronics and hardware interfaces?

Yes.

Doly's electronics schematics and other hardware design files are published in the DOLY-DIY GitHub repository. They are useful for developers who need to understand the underlying sensors, motor drivers, audio hardware, expansion interfaces, power rails, and board connections.

See the [Doly Electronics directory](https://github.com/robotdoly/DOLY-DIY/tree/main/Electronics) and [schematics](https://github.com/robotdoly/DOLY-DIY/tree/main/Electronics/Schematic).

---

### Can I connect additional hardware such as USB LiDAR, a depth camera, microphone array, or other sensors?

Yes. Doly exposes hardware expansion interfaces for custom devices and sensors.

Available expansion interfaces include:

- 6 GPIO pins
- Qwiic / I2C
- UART
- USB
- 2 servo/PWM outputs
- 2 analog inputs associated with the servo channels

The two analog inputs are normally used by Doly's servo system for servo-position feedback. If the corresponding servo channel is not being used through the ServoControl SDK, the analog input can be repurposed as an independent analog input for a custom project.

The analog inputs are separate from the six digital GPIO pins. The current public `IoControl` API documents digital GPIO read/write for pins 0-5; it does not currently expose those servo analog lines as generic `IoControl` analog-read functions.

For power, the shared **5 V user supply supports 1.5 A continuous current and up to 2.15 A peak current**. The peak figure should not be treated as a continuous operating budget. When adding devices such as LiDAR, depth cameras, microphone arrays, servos, or other USB/peripheral hardware, developers should total the expected current consumption and leave appropriate operating margin.

For pinouts, power rails, and electrical details, see:

- [Doly IO schematic](https://github.com/robotdoly/DOLY-DIY/blob/main/Electronics/Schematic/Schematic_IO.pdf)
- [Doly Main Board schematic](https://github.com/robotdoly/DOLY-DIY/blob/main/Electronics/Schematic/Schematic_MainBoard.pdf)
- [Doly Developer page](https://doly.ai/developer/)
- [IoControl public header](https://github.com/robotdoly/DOLY-DIY/blob/main/SDK/include/IoControl.h)
- [ServoControl public header](https://github.com/robotdoly/DOLY-DIY/blob/main/SDK/include/ServoControl.h)

---

### Which I2C buses and device addresses are used inside Doly, and which bus should I use for custom I2C hardware?

Doly uses multiple I2C buses so that different groups of internal hardware are separated.

The current Doly bus map is:

| Linux I2C bus | Doly devices | I2C address |
|---|---|---|
| `/dev/i2c-6` | 2 × ST VL6180X Time-of-Flight sensors | `0x29` and `0x30` after initialization |
| `/dev/i2c-6` | User / Qwiic I2C expansion | Address depends on the connected device |
| `/dev/i2c-3` | NXP PCA9535 GPIO expander | `0x20` |
| `/dev/i2c-3` | NXP PCA9685 PWM controller | `0x40` |
| `/dev/i2c-3` | TI INA219 current/power monitor | `0x41` |
| `/dev/i2c-1` | Texas Instruments TLV320AIC3110 audio codec | `0x18` |
| `/dev/i2c-1` | ST LSM6DSR 6-axis IMU | `0x6A` |
| `/dev/i2c-1` | Texas Instruments ADS1015 ADC | `0x48` |
| `/dev/i2c-0` | Sony IMX219 camera control | Managed by the Raspberry Pi camera/GPU subsystem |

For custom I2C sensors or other expansion hardware, **`/dev/i2c-6` is the intended user-accessible I2C/Qwiic bus**. Developers should make sure that a new device does not conflict with an address already used on that bus.

The two VL6180X ToF sensors have the same default I2C address (`0x29`). During Doly initialization, one sensor is reassigned to `0x30` so that both sensors can operate on the same I2C bus.

The other I2C buses are used by Doly's internal hardware. In particular, `/dev/i2c-0` is used for camera control by the Raspberry Pi camera/GPU subsystem and should not be treated as the normal user expansion bus.

Hardware references:

- [Doly Main Board BOM](https://github.com/robotdoly/DOLY-DIY/blob/main/Electronics/BOM/BOM_MainBoard.csv)
- [Doly Main Board schematic](https://github.com/robotdoly/DOLY-DIY/blob/main/Electronics/Schematic/Schematic_MainBoard.pdf)
- [Doly IO schematic](https://github.com/robotdoly/DOLY-DIY/blob/main/Electronics/Schematic/Schematic_IO.pdf)
- [ST VL6180X](https://www.st.com/en/imaging-and-photonics-solutions/vl6180x.html)
- [NXP PCA9535](https://www.nxp.com/products/interfaces/ic-spi-i3c-interface-devices/general-purpose-i-o-gpio/16-bit-ic-bus-and-smbus-low-power-i-o-port-with-interrupt:PCA9535_PCA9535C)
- [NXP PCA9685](https://www.nxp.com/products/power-drivers/lighting-driver-and-controller-ics/led-drivers/16-channel-12-bit-pwm-fm-plus-ic-bus-led-driver:PCA9685)
- [Texas Instruments INA219](https://www.ti.com/product/INA219)
- [ST LSM6DSR](https://www.st.com/en/mems-and-sensors/lsm6dsr.html)
- [Texas Instruments ADS1015](https://www.ti.com/product/ADS1015)
- [Texas Instruments TLV320AIC3110](https://www.ti.com/product/TLV320AIC3110)

---

### What might a custom AI application look like?

The simplest way to think about it is that **your custom Python or C++ application sits in the middle**.

It talks to Doly's hardware through the Doly SDK and standard Linux interfaces such as ALSA, and it talks to an AI service such as OpenAI or Gemini through that service's API.

```text
Doly microphone / camera / sensors
              ↓
      Your custom application
          (Python / C++)
              ↓
       OpenAI / Gemini API
              ↓
      Your custom application
              ↓
Doly speakers / eyes / arms / motors
```

For example, Doly can hear a question through its microphones, your application can process the audio locally or send audio/text to an AI service, receive the response, and then use Doly's speakers, eyes, arms, LEDs, or motors to react.

The same architecture can also be completely local. Instead of a cloud AI service, the custom application can communicate with software running locally on Doly or another computer on the LAN.

This is only one possible design. Developers are free to build the AI, networking, audio, and robot-control flow that fits their application.

---

For implementation details, see the Doly SDK documentation, examples, electronics schematics, and community discussions linked above.
