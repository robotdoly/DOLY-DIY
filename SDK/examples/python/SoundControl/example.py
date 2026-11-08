"""
example.py

It demonstrates:
- Initializing the Sound controller
- Registering the static event listener
- Setting volume
- Playing sound
- Cleaning up (dispose)

"""

import time
from pathlib import Path
import doly_sound as snd


def on_snd_begin(id: int, volume):
    print(f"[info] Sound begin id={id} volume={volume:.3f}")

def on_snd_complete(id: int):
    print(f"[info] Sound completed id={id}")

def on_snd_abort(id: int):
    print(f"[info] Sound aborted id={id}")

def on_snd_error(id: int):
    print(f"[error] Sound error id={id}")

def main():

    # Version
    try:
        print(f"[info] SoundControl Version: {snd.get_version():.3f}")
    except AttributeError:
        pass



    # Initialize sound controler
    rc = snd.init()
    if rc < 0:
        print(f"[error] SoundControl.init failed rc={rc}")
        return -2

    result = 0
    try:
        # Register static event listeners if required
        snd.on_begin(on_snd_begin)
        snd.on_complete(on_snd_complete)
        snd.on_abort(on_snd_abort)
        snd.on_error(on_snd_error)

        # Set volume to 80%
        rc = snd.set_volume(80)
        if rc < 0:
            print(f"[error] Set volume failed rc={rc}")
            return -3

        # Play sound
        sound_path = Path(__file__).with_name("sound_test.wav")
        rc = snd.play(str(sound_path), 1)
        if rc < 0:
            print(f"[error] Play failed rc={rc}")
            return -3

        # Wait until done (simple polling example)
        while snd.get_state() != snd.SoundState.Stop:
            time.sleep(0.1)
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
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
