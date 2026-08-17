# Using Doly with OpenAI, Gemini, and Other AI Models

## Developer FAQ

Doly can be used as a programmable robot platform for custom AI applications. Developers can stop the standard Doly service, run their own application directly on Doly's Raspberry Pi, use the Doly SDK for robot control, and use standard Linux interfaces such as ALSA for audio.

The AI portion of the application can then be implemented by the developer using the AI model or service that fits the project, such as OpenAI, Google Gemini, or another service.

> This FAQ describes custom developer use. It does not mean that every AI provider has an official Doly integration.

---

### Can I use Doly with OpenAI, Gemini, or another AI model?

Yes. You can run your own C++ or Python application directly on Doly and build the AI integration inside that application.

Doly's SDK can be used for robot functions, while your application handles communication with the AI model or service you choose.

---

### Do I have to use Doly's default personality or software?

No. Doly's standard services can be disabled, and you can run your own software instead.

You can also configure your own application or service to start automatically when Doly boots.

---

### Does my custom AI application need the Doly mobile app or Doly cloud service?

No for local robot control. Doly is designed to work offline, and a custom application can control the robot locally.

If the AI model you choose is a cloud-based service, your own application will of course need the network connection required by that service.

---

### Can a Python application access Doly's camera?

Yes. The C++ or Python SDK provides full camera-frame access, not only snapshots.

This allows a custom application to process camera frames itself or send selected image data to its own vision or AI pipeline.

---

### Can I access the onboard microphones directly?

Yes. With the Doly service stopped, the onboard microphones can be accessed through ALSA or an ALSA-based library.

A custom application can continuously capture and process raw microphone audio.

---

### Can microphone capture run while I control other parts of Doly?

Yes.

Doly libraries/APIs can operate on different threads. A custom application can capture microphone audio while also using Doly APIs for functions such as the camera, drive system, arms, eyes/displays, and sensors.

---

### Can Doly record from the microphones while playing audio through the speakers?

Yes. Full-duplex audio operation is possible.

A custom application can capture microphone audio while simultaneously playing arbitrary audio through Doly's speakers.

---

### Does Doly have hardware Acoustic Echo Cancellation (AEC)?

No. Doly uses the Texas Instruments TLV320AIC3110 audio codec, which does not provide on-chip hardware Acoustic Echo Cancellation.

Doly's standard software uses third-party libraries for acoustic echo cancellation and noise processing, but these libraries are not currently exposed through the Doly SDK.

For a custom application, developers can select their own software AEC and noise-processing solution. There are many open-source options available.

The Doly electronics schematics are also available in the GitHub repository for developers who want to examine the audio hardware in more detail.

---

### Can I use my own speech recognition, TTS, or voice system?

Yes.

After disabling the standard Doly services, you can use your own audio, speech-recognition, or text-to-speech solution.

You are not required to use Doly's built-in TTS system for a custom application.

---


### Can different Doly functions run at the same time?

Yes.

The libraries/APIs can be used on different threads within a custom application, allowing an AI program to listen, see, process sensor information, and control robot functions concurrently.

---

### Can I continuously run my own application on Doly?

Yes.

Doly runs a Linux/Debian-based operating system on its Raspberry Pi. You can disable Doly's standard services and configure your own application or Linux service to start automatically at boot.

---

### Can Doly charge while my custom software is running?

Yes.

Charging is controlled at the hardware level and does not depend on software.

Doly can remain physically docked and charge normally while the standard Doly service is stopped and your custom C++ or Python application continues running.

---

### Can I inspect Doly's electronics and hardware interfaces?

Yes.

Doly's electronics schematics are shared in the GitHub repository. This is useful for developers who want to understand the underlying sensors, motor drivers, audio codec, and other hardware used by Doly.

---

### What might a custom AI application look like?

The simplest way to think about it is that **your custom Python or C++ application sits in the middle**.

It talks to Doly's hardware through the Doly SDK and ALSA, and it talks to an AI service such as OpenAI or Gemini through that service's API.

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

For example, Doly can hear a question through its microphones, your application can send the audio or text to the AI service, receive the response, and then use Doly's speakers, eyes, arms, or motors to react.

This is only one possible design. Developers are free to build the AI, networking, audio, and robot-control flow that fits their application.

---

See the Doly SDK documentation, examples, electronics schematics, and GitHub discussions for more implementation details.

