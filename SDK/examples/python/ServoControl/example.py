"""
example.py

It demonstrates:
- Initializing the external servo controller (IO plug servos)
- Setting static event callbacks
- Setting servo position
- Releasing servo hold
- Cleaning up (dispose)

"""

import time
import doly_servo as servo

def on_servo_abort(id, channel):
    print(f"[info] Servo abort id:{id} channel:{channel}")

def on_servo_complete(id, channel):
    print(f"[info] Servo complete id:{id} channel:{channel}")

def on_servo_error(id, channel):
    print(f"[error] Servo error id:{id} channel:{channel}")

def main():

    # Version
    try:
        print(f"[info] ServoControl Version: {servo.get_version():.3f}")
    except AttributeError:
        pass


    # Initialize servo controler
    rc = servo.init()
    if rc < 0:
        print(f"[error] ServoControl.init failed rc={rc}")
        return -2

    result = 0
    attempted_channels = []
    try:
        # Add static event listeners
        servo.on_complete(on_servo_complete)
        servo.on_abort(on_servo_abort)
        servo.on_error(on_servo_error)

        # Set positions on both external servos
        attempted_channels.append(servo.ServoId.Servo0)
        rc = servo.set_servo(1, servo.ServoId.Servo0, 180, 100, False)
        if rc < 0:
            print(f"[error] Set Servo0 failed rc={rc}")
            return -3
        attempted_channels.append(servo.ServoId.Servo1)
        rc = servo.set_servo(2, servo.ServoId.Servo1, 90, 1, False)
        if rc < 0:
            print(f"[error] Set Servo1 failed rc={rc}")
            return -3

        # Preserve the original dwell; it does not guarantee both servos have completed.
        time.sleep(3)
    finally:
        # Release hold on each attempted channel, including after a partial command failure.
        # Busy (-2) is a failure: do not extend the dwell or add a completion wait.
        for channel in attempted_channels:
            try:
                cleanup_rc = servo.release(channel)
                if cleanup_rc < 0:
                    print(f"[error] Servo release failed channel={channel} rc={cleanup_rc}")
                    result = 1
            except Exception as exc:
                print(f"[error] Servo release raised channel={channel}: {exc}")
                result = 1
        try:
            cleanup_rc = servo.dispose()
            if cleanup_rc < 0:
                print(f"[error] servo.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] servo.dispose raised: {exc}")
            result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
