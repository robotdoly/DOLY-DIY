# FanControl (Python)

## Import

```python
import doly_fan
```

## Minimal working example

Below is the module’s `examples/python/FanControl/example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Initializing the Fan controller (manual mode)
- Setting Fan speed
- Cleaning up (dispose)

"""

import time
import doly_fan as fan

def main():

    # Version
    try:
        print(f"[info] FanControl Version: {fan.get_version():.3f}")
    except AttributeError:
        pass


    # Initialize Fan Control
    # auto_control = False: we will control fan speed manually in this example
    # auto_control = True: automatic fan control will try to manage CPU temperature under 70 degree Celsius
    rc = fan.init(False)
    if rc < 0:
        print(f"[error] FanControl.init failed rc={rc}")
        return -2

    result = 0
    stopped = False
    try:
        # Set fan speed to 100%
        rc = fan.set_fan_speed(100)
        if rc < 0:
            print(f"[error] Set fan speed failed rc={rc}")
            return -3
        print("[info] Fan speed set to 100%")

        # wait for X seconds to test it
        time.sleep(7)

        #Turn off fan
        rc = fan.set_fan_speed(0)
        if rc < 0:
            print(f"[error] Stop fan failed rc={rc}")
            return -3
        stopped = True
        print("[info] Fan turned off")
    finally:
        if not stopped:
            try:
                cleanup_rc = fan.set_fan_speed(0)
                if cleanup_rc < 0:
                    print(f"[error] Fan stop during cleanup failed rc={cleanup_rc}")
                    result = 1
            except Exception as exc:
                print(f"[error] Fan stop during cleanup raised: {exc}")
                result = 1
        try:
            cleanup_rc = fan.dispose()
            if cleanup_rc < 0:
                print(f"[error] fan.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] fan.dispose raised: {exc}")
            result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Initialize**
- **Set fan speed**
- **Dispose / cleanup**


## Notes

This module uses `/dev/doly_*` devices. Permissions are configured by default; however, if you encounter issues, ensure user permissions are correctly set (see **Troubleshooting → Permissions**).


## API reference

See: [API reference](../api/fan_control.md)

