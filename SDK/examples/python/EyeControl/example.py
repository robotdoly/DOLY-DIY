"""
example.py

It demonstrates:
- Initializing the eye controller
- Registering the static event listeners
- Changing Iris and color
- Animating eyes with predefined animations
- Using a custom image for eye parts
- Setting/getting position of iris
- Cleaning up (dispose)

"""

import time
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


    # Initialize EyeControl
    rc = eye.init(ColorCode.Blue, ColorCode.White)
    if rc < 0:
        print("[error] EyeControl init failed")
        return -2

    result = 0
    try:
        # Register static event listeners
        eye.on_start(on_eye_start)
        eye.on_complete(on_eye_complete)
        eye.on_abort(on_eye_abort)

        time.sleep(3)

        # Example: Change iris
        rc = eye.set_iris(eye.IrisShape.Modern, ColorCode.DarkGreen, eye.EyeSide.Both)
        if rc < 0:
            print(f"[error] Set iris failed rc={rc}")
            return -3

        time.sleep(3)

        # Example: Set Animation (non-blocking)
        rc = eye.set_animation(1, eye.expressions.EXCITED)
        if rc < 0:
            print(f"[error] Set animation failed rc={rc}")
            return -3

        # Wait until animation complete (same as while (EyeControl::isAnimating()))
        while eye.is_animating():
            time.sleep(0.01)

        time.sleep(3)

        # Example: Using custom images for lids, iris or background
        visual = VContent.get_image("/.doly/images/lids/11B.png", True, True)

        if not visual.is_ready():
            print("[error] image load failed!")
            return -3
        # Bottom lid. Assignment errors propagate through finally; no further image operations.
        eye.set_lid_image(visual, False, eye.EyeSide.Both)

        # EyeControl::setIrisPosition(EyeSide::BOTH, 120, 120, 1.2, 1.2, 0, 180);
        rc = eye.set_position(eye.EyeSide.Both, 120, 120, 1.2, 1.2, 0, 180)
        if rc < 0:
            print(f"[error] Set iris position failed rc={rc}")
            return -3

        # EyeControl::getIrisPosition(EyeSide::LEFT, x, y);
        rc, x, y = eye.get_iris_position(eye.EyeSide.Left)
        if rc < 0:
            print(f"[error] Get iris position failed rc={rc}")
            return -3
        print(f"[info] Left iris position: {x},{y} (rc={rc})")
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
        try:
            # Callback cleanup only: no renderer shutdown, image release or worker join.
            eye.dispose()
        except Exception as exc:
            print(f"[error] eye.dispose raised: {exc}")
            result = 1

    return result


if __name__ == "__main__":
    raise SystemExit(main())
