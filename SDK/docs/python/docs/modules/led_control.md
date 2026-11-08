# LedControl (Python)

## Import

```python
import doly_led
```

## Minimal working example

Below is the module’s `examples/python/LedControl/example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Initializing the LED controller
- Setting static event callbacks
- Change LEDs with fade effect
- Change LEDs instant
- Cleaning up (dispose)

"""

import time
import doly_led as led
from doly_color import Color, ColorCode

def on_led_complete(cmd_id: int, side):
    print(f"[info] Led complete id={cmd_id} side={side}")

def on_led_error(cmd_id: int, side, err_type):
    print(f"[error] Led error id={cmd_id} side={side} error={err_type}")

def main():

    # Version
    try:
        print(f"[info] LedControl Version: {led.get_version():.3f}")
    except AttributeError:
        pass



    # Initialize LED controller
    rc = led.init()
    if rc < 0:
        print(f"[error] LedControl.init failed rc={rc}")
        return -2

    result = 0
    try:
        # Register static event listeners if required
        led.on_complete(on_led_complete)
        led.on_error(on_led_error)

        # fade effect example
        activity1 = led.LedActivity()
        activity1.mainColor = Color.from_code(ColorCode.Red)
        activity1.fadeColor = Color.from_code(ColorCode.Blue)
        activity1.fade_time = 2000  # 2 seconds (milliseconds)

        led.process_activity(1, led.LedSide.Both, activity1)
        # Let the activity run for a while on another thread
        time.sleep(2.5)

        # instant color change example
        activity2 = led.LedActivity()
        activity2.mainColor = Color.from_code(ColorCode.Orange)
        activity2.fade_time = 0

        led.process_activity(2, led.LedSide.Right, activity2)

        # wait activity event completion before dispose
        time.sleep(0.2)
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
        try:
            cleanup_rc = led.dispose()
            if cleanup_rc < 0:
                print(f"[error] led.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] led.dispose raised: {exc}")
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
- **Set LED(s) color**
- **Read state / values**
- **Dispose / cleanup**


## API reference

See: [API reference](../api/led_control.md)

