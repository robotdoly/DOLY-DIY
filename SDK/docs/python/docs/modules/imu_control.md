# ImuControl (Python)

## Import

```python
import doly_imu
```

## Minimal working example

Below is the module’s `examples/python/ImuControl/example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Initializing the IMU controller
- Registering the static event listeners
- Wait for a while to test sensor events
- Cleaning up (dispose)

"""

import time
import doly_helper as helper
import doly_imu as imu

def on_imu_update(data):
    print(f"[info] yaw:{data.ypr.yaw} pitch:{data.ypr.pitch} roll:{data.ypr.roll}")

def on_imu_gesture(gesture, direction):
    print(f"[warn] gesture:{gesture} direction:{direction}")

def main():

    # Version
    try:
        print(f"[info] ImuControl Version: {imu.get_version():.3f}")
    except AttributeError:
        pass


    # For better performance provide actual calibrated IMU offsets or calculate them once after initialization
    # and use them for next initializations.
    # in this example we will use Helper to get previously saved offsets

    # Read settings
    # one time read is sufficient for the lifetime of the application
    if helper.read_settings() < 0:
        print("[error] Read settings failed")
        return -1

    # Get pre defined IMU offsets
    rc, gx, gy, gz, ax, ay, az = helper.get_imu_offsets()
    if rc < 0:
        print(f"[error] Get IMU offsets failed rc={rc}")
        return -2

    print(f"[info] gx:{gx} gy:{gy} gz:{gz} ax:{ax} ay:{ay} az:{az}")

    # Initialize IMU controler
    # delay 1 second before processing events
    rc = imu.init(1, gx, gy, gz, ax, ay, az)
    if rc < 0:
        print(f"[error] ImuControl.init failed rc={rc}")
        return -3

    result = 0
    try:
        # register static event call backs
        imu.on_update(on_imu_update)
        imu.on_gesture(on_imu_gesture)

        # wait for a while before exiting
        time.sleep(10)
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
        try:
            cleanup_rc = imu.dispose()
            if cleanup_rc < 0:
                print(f"[error] imu.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] imu.dispose raised: {exc}")
            result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Get offset values**
- **Initialize**
- **Add event listener**
- **Read state / values**
- **Dispose / cleanup**


## API reference

See: [API reference](../api/imu_control.md)

