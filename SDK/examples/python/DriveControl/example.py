"""
example.py

It demonstrates:
- Initializing the drive controller
- Registering the static event listener
- Sending drive commands using multiple options
- Reading the current estimated position
- Cleaning up

"""


import time
import doly_drive as drive


def on_drive_complete(cmd_id: int):
    print(f"[info] Drive complete id={cmd_id}")


def on_drive_error(cmd_id: int, side, err_type):
    # side is DriveMotorSide enum, err_type is DriveErrorType enum
    print(f"[error] Drive error id={cmd_id} side={int(side)} type={int(err_type)}")


def on_drive_state_change(drive_type, state):
    print(f"[info] Drive state type={int(drive_type)} state={int(state)}")


def main():
    # Version
    try:
        print(f"[info] DriveControl Version: {drive.get_version():.3f}")
    except AttributeError:
        pass


    result = 0
    initialized = False
    registered = []
    try:
        # Register static event listeners
        drive.on_complete(on_drive_complete)
        registered.append(drive.on_complete)
        drive.on_error(on_drive_error)
        registered.append(drive.on_error)
        drive.on_state_change(on_drive_state_change)
        registered.append(drive.on_state_change)

        # Initialize DriveControl with zero IMU offsets (example)
        # For better performance provide actual calibrated IMU offsets.
        # Check ../Helper/example.py for more information
        rc = drive.init()
        if rc < 0:
            print("[error] DriveControl init failed")
            return -2
        initialized = True

        speed = 50  # example speed % (0..100)

        # Example 1: go 100mm forward, brake at end
        if not drive.go_distance(1, 100, speed, True, True):
            print("[error] Drive distance command rejected")
            return -3

        # Example 2: Rotate 45 degrees counterclockwise on wheel, with brake at end
        # drive.go_rotate(2, -45.0, False, speed, True, True)

        # Example 3: Go to X= -100mm, Y= 200mm, approaching forward with acceleration, without brake at end
        # drive.go_xy(1, -100, 200, speed, True, False, 50)

        # Example 4: Free drive left and right wheels forward at specified speed for 2 seconds
        # drive.free_drive(speed, False, True)  # Low-level drive right
        # drive.free_drive(speed, True, True)   # Low-level drive left
        # time.sleep(2.0)
        # Stop driving wheels
        # drive.free_drive(0, False, True)
        # drive.free_drive(0, True, True)

        # Wait until done (simple polling example)
        while True:
            state = drive.get_state()
            if state != drive.DriveState.Running:
                break
            time.sleep(0.05)
        if state == drive.DriveState.Error:
            print("[error] Drive movement ended in Error")
            return -3

        # Get final position
        pos = drive.get_position()
        print(f"[info] Robot pos x={pos.x} y={pos.y} head={pos.head}")
    finally:
        if initialized:
            # Also disposes native IMU. The None adapter cannot prove cleanup success.
            try:
                drive.dispose(True)
            except Exception as exc:
                print(f"[error] drive.dispose raised: {exc}")
                result = 1
        else:
            # Initialization failed: remove only registrations installed by this example.
            for unregister in registered:
                try:
                    unregister(None)
                except Exception as exc:
                    print(f"[error] Callback removal raised: {exc}")
                    result = 1

    return result


if __name__ == "__main__":
    raise SystemExit(main())
