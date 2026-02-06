# DriveControl (Python)



## Import

```python
import doly_drive
```

## Minimal working example

Below is the module’s `example.py` as a starting point.

```python
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
import doly_helper as helper


def on_drive_complete(cmd_id: int):
    print(f"[info] Drive complete id={cmd_id}")


def on_drive_error(cmd_id: int, side, err_type):
    # side is DriveErrorSide enum, err_type is DriveErrorType enum
    print(f"[error] Drive error id={cmd_id} side={int(side)} type={int(err_type)}")


def on_drive_state_change(drive_type, state):
    print(f"[info] Drive state type={int(drive_type)} state={int(state)}")


def main():
    # Version
    try:
        print(f"[info] DriveControl Version: {drive.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1

    # Register static event listeners 
    drive.on_complete(on_drive_complete)
    drive.on_error(on_drive_error)
    drive.on_state_change(on_drive_state_change)

    # Initialize DriveControl with zero IMU offsets (example)
    # For better performance provide actual calibrated IMU offsets.
    # Check examples_helper.py for more information
    rc = drive.init()
    if rc != 0:
        print("[error] DriveControl init failed")
        return -2

    speed = 50  # example speed % (0..100)

    # Example 1: go 100mm forward, brake at end
    drive.go_distance(1, 100, speed, True, True)

    # Example 2: Rotate 45 degrees counterclockwise on wheel, with brake at end
    # drive.go_rotate(2, -45.0, False, speed, True, True)

    # Example 3: Go to X= -100mm, Y= 200mm, approaching forward with acceleration, without brake at end
    # drive.go_xy(1, -100.0, 200.0, speed, True, False, 50)

    # Example 4: Free drive left and right wheels forward at specified speed for 2 seconds
    # drive.free_drive(speed, False, True)  # Low-level drive right
    # drive.free_drive(speed, True, True)   # Low-level drive left
    # time.sleep(2.0)
    # Stop driving wheels
    # drive.free_drive(0, False, True)
    # drive.free_drive(0, True, True)

    # Wait until done (simple polling example)
    while drive.get_state() == drive.DriveState.Running:
        time.sleep(0.05)

    # Get final position
    pos = drive.get_position()
    print(f"[info] Robot pos x={pos.x} y={pos.y} head={pos.head}")

    # Cleanup
    drive.dispose(True)  # dispose IMU as well   

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Initialize**
- **Add event listener**
- **Set drive command**
- **Read state / values**
- **Dispose / cleanup**

## Notes

- **Platform:** Raspberry Pi OS
- **Python:** 3.11
- **Installed on robot:** Yes (preinstalled)

This module uses `/dev/doly_*` devices. Permissions are configured by default; however, if you encounter issues, ensure user permissions are correctly set (see **Troubleshooting → Permissions**).

## API reference

See: [API reference](../api/drive_control.md)
