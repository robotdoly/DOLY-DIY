# SoundControl (Python)



## Import

```python
import doly_helper
```

## Minimal working example

Below is the module’s `example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Initializing the Sound controller
- Registering the static event listener
- Seting volume
- Playin sound
- Cleaning up (dispose)

"""

import time
import doly_helper as helper
import doly_sound as snd


def on_snd_begin(id: int, volume):
    print(f"[info] Sound begin id={id} side={volume:.3f}")

def on_snd_complete(id: int):
    print(f"[info] Sound completed id={id}")

def on_snd_abort(id: int):
    print(f"[info] Sound aborted id={id}")

def on_snd_error(id: int):
    print(f"[error] Sound error id={id}")

def main():

    # Version
    try:
        print(f"[info] LedControl Version: {snd.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
        

    # Initialize sound controler
    rc = snd.init()
    if rc < 0:
        print(f"[error] SoundControl.init failed rc={rc}")
        return -2
    
    # Register static event listeners if required
    snd.on_begin(on_snd_begin)
    snd.on_complete(on_snd_complete)
    snd.on_abort(on_snd_error)
    snd.on_error(on_snd_error)

	# Set volume to 80%
    snd.set_volume(80)

    # Play sound
    rc = snd.play("sound_test.wav", 1) < 0
    if rc < 0:
        print(f"[error] Play failed rc={rc}")
        return -3

    # Wait until done (simple polling example)
    while snd.get_state() != snd.SoundState.Stop:
        time.sleep(0.1)

    # Cleanup
    snd.dispose()    
    time.sleep(0.2) 

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Initialize**
- **Add event listener**
- **Play**
- **Read state / values**
- **Dispose / cleanup**

## Notes

- **Platform:** Raspberry Pi OS
- **Python:** 3.11
- **Installed on robot:** Yes (preinstalled)

## API reference

See: [API reference](../api/sound_control.md)
