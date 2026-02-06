# ImuControl (Python)



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

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
    
    # For better performance provide actual calibrated IMU offsets or calculate them once after initialization
	# and use them for next initializations.
	# in this example we will use Helper to get previously saved offsets 

	# Read settings
	# one time read is sufficient for the lifetime of the application
    if helper.read_settings() < 0: 
        print("[error] Doly service stop failed")
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
    
    # register static event call backs
    imu.on_update(on_imu_update)
    imu.on_gesture(on_imu_gesture)

    # wait for a while before exiting
    time.sleep(10)

    # Cleanup
    imu.dispose() 
    time.sleep(0.2) 

    return 0


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

## Notes

- **Platform:** Raspberry Pi OS
- **Python:** 3.11
- **Installed on robot:** Yes (preinstalled)

## API reference

See: [API reference](../api/imu_control.md)
