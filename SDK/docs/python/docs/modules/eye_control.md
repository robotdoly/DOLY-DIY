# EyeControl (Python)



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
- Initializing the eye controller
- Registering the static event listeners
- Changing Iris and color
- Animating eyes with predefined animations
- Using a custom image for eye parts
- Setting/getting position of iris 
- Cleaning up (remove_listener + dispose)

"""

import time
import doly_helper as helper
import doly_eye as eye
from doly_color import ColorCode
from doly_vcontent import VContent


def on_eye_start(anim_id: int):
    print(f"[info] Eye animation start id:{anim_id}")


def on_eye_complete(anim_id: int):
    print(f"[info] Eye animation complete id:{anim_id}")


def on_eye_abort(anim_id: int):
    print(f"[warn] Eye animation abort id:{anim_id}")


def main():

    # Version
    try:
        print(f"[info] EyeControl Version:{eye.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT ***
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict
    if helper.stop_doly_service() < 0:
        print("[error] Doly service stop failed")
        return -1

    # Initialize EyeControl
    rc = eye.init(ColorCode.Blue, ColorCode.White)
    if rc != 0:
        print("[error] EyeControl init failed")
        return -2

    # Register static event listeners
    eye.on_start(on_eye_start)
    eye.on_complete(on_eye_complete)
    eye.on_abort(on_eye_abort)

    time.sleep(3)

    # Example: Change iris
    eye.set_iris(eye.IrisShape.Modern, ColorCode.DarkGreen, eye.EyeSide.Both)

    time.sleep(3)

    # Example: Set Animation (non-blocking)
    eye.set_animation(1, eye.expressions.EXCITED)

    # Wait until animation complete (same as while (EyeControl::isAnimating()))
    while eye.is_animating():
        time.sleep(0.01)

    time.sleep(3)

    # Example: Using custom images for lids, iris or background
    visual = VContent.get_image("/.doly/images/lids/11B.png", True, True)

    if not visual.is_ready():
        print("[error] image load failed!")
    else:
        # EyeControl::setLid(&visual, false, EyeSide::BOTH);  (bottom lid)
        ret = eye.set_lid_image(visual, False, eye.EyeSide.Both)
        if ret < 0:
            print(f"[error] Set eye lid failed err:{ret}")

    # EyeControl::setPosition(EyeSide::BOTH, 120, 120, 1.2, 1.2, 0, 180);
    eye.set_position(eye.EyeSide.Both, 120, 120, 1.2, 1.2, 0, 180)

    # EyeControl::getIrisPosition(EyeSide::LEFT, x, y);
    rc, x, y = eye.get_iris_position(eye.EyeSide.Left)
    print(f"[info] Left iris position: {x},{y} (rc={rc})")

    # Cleanup callbacks (optional)
    eye.dispose()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Initialize**
- **Add event listeners for animations**
- **Customize eyes**
- **Set animation**
- **Dispose / cleanup**

## Notes

- **Platform:** Raspberry Pi OS
- **Python:** 3.11
- **Installed on robot:** Yes (preinstalled)

This module uses `/dev/doly_*` devices. Permissions are configured by default; however, if you encounter issues, ensure user permissions are correctly set (see **Troubleshooting → Permissions**).

## API reference

See: [API reference](../api/eye_control.md)
