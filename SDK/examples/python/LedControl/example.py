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
import doly_helper as helper
import doly_led as led
from doly_color import Color, ColorCode

def on_led_complete(cmd_id: int, side):
    print(f"[info] Led complete id={cmd_id} side={side}")

def on_led_error(cmd_id: int, side, err_type):
    print(f"[info] Led complete id={cmd_id} side={side} error={err_type}")

def main():

    # Version
    try:
        print(f"[info] LedControl Version: {led.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
        

    # Initialize ToF sensor controler
    rc = led.init()
    if rc != 0:
        print(f"[error] LcdControl.init failed rc={rc}")
        return -2
    
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
    activity1.fade_time = 0  

    led.process_activity(2, led.LedSide.Right, activity2)

    # wait activity event completion before dispose
    time.sleep(0.2)

    # Cleanup
    led.dispose()    
    time.sleep(0.2) 

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
