# TouchControl (Python)

## Import

```python
import doly_touch
```

## Minimal working example

Below is the module’s `examples/python/TouchControl/example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Initializing the touch controller
- Registering the static event listeners
- Cleaning up (dispose)

"""


import time
import doly_touch as touch

def on_touch(side, state):
    print(f"[info] Touch Event side={side} state={state}")

def on_touch_activity(side, activity):
    print(f"[info] Touch Activity side={side} type={activity}")

def main():

    # Version
    try:
        print(f"[info] TouchControl Version: {touch.get_version():.3f}")
    except AttributeError:
        pass



    # Initialize TouchControl
    rc = touch.init()
    if rc < 0:
        print(f"[error] TouchControl.init failed rc={rc}")
        return -2

    result = 0
    try:
        # Register static event listeners
        touch.on_touch(on_touch)
        touch.on_touch_activity(on_touch_activity)

        # Run for 30 seconds to test touch events
        print(f"[info] Run for 30 seconds to test touch events")
        time.sleep(30)
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
        try:
            cleanup_rc = touch.dispose()
            if cleanup_rc < 0:
                print(f"[error] touch.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] touch.dispose raised: {exc}")
            result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Initialize**
- **Add event listener**
- **Read state / values**
- **Dispose / cleanup**


## API reference

See: [API reference](../api/touch_control.md)

