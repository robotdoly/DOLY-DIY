# TtsControl (Python)

## Import

```python
import doly_tts
```

## Minimal working example

Below is the module’s `examples/python/TtsControl/example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Initializing the TTS (text to speech) controller
- Produce sound
- Initializing the sound controller
- Playing sound
- Cleaning up (dispose)

"""

import time
import doly_sound as snd
import doly_tts as tts

def main():

    # Version
    try:
        print(f"[info] TtsControl Version: {tts.get_version():.3f}")
    except AttributeError:
        pass


    # Initialize tts controler
    print("[info] Initialize and load voice model...")
    rc = tts.init(tts.VoiceModel.Model1, "output.wav")
    if rc < 0:
        print(f"[error] TtsControl.init failed rc={rc}")
        return -2

    result = 0
    sound_initialized = False
    try:
        # Generate speech sound file from text
        print("[info] Produce Text to Speech")
        rc = tts.produce("Hello, this is a text to speech test example.")
        if rc < 0:
            print(f"[error] Produce speech failed rc={rc}")
            return -3

        # Initialize sound controler to test sound file
        rc = snd.init()
        if rc < 0:
            print(f"[error] SoundControl.init failed rc={rc}")
            return -3
        sound_initialized = True

        # Play sound
        rc = snd.play("output.wav", 1)
        if rc < 0:
            print(f"[error] Play failed rc={rc}")
            return -3

        # Preserve the original five-second playback interval; completion is not polled.
        time.sleep(5)
    finally:
        # Preserve TTS-before-Sound cleanup; attempt both even if one fails.
        try:
            cleanup_rc = tts.dispose()
            if cleanup_rc < 0:
                print(f"[error] tts.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] tts.dispose raised: {exc}")
            result = 1
        if sound_initialized:
            try:
                cleanup_rc = snd.dispose()
                if cleanup_rc < 0:
                    print(f"[error] snd.dispose failed rc={cleanup_rc}")
                    result = 1
            except Exception as exc:
                print(f"[error] snd.dispose raised: {exc}")
                result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Initialize**
- **Produce sound**
- **Play sound**
- **Dispose / cleanup**

## API reference

See: [API reference](../api/tts_control.md)

