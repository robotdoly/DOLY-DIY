"""
example.py

It demonstrates:
- Initializing the LCD controller
- Writes a full white frame to both displays
- Fills left display blue and right display red
- Cleaning up (dispose)

"""

import time
import doly_helper as helper
import doly_lcd as lcd

def fill_buffer_example():
    # Allocate the exact buffer size required by the LCD
    size = lcd.get_buffer_size()
    buf = bytearray([0xFF]) * size 

    # write_lcd(side, buffer) accepts bytes/bytearray/memoryview/numpy uint8
    r1 = lcd.write_lcd(lcd.LcdSide.Left, buf)
    r2 = lcd.write_lcd(lcd.LcdSide.Right, buf)

    if r1 < 0:
        print("Left LCD write failed!")
    if r2 < 0:
        print("Right LCD write failed!")

    return 0 if (r1 >= 0 and r2 >= 0) else -1

def fill_color_example():
    lcd.lcd_color_fill(lcd.LcdSide.Left, 0, 0, 255)
    lcd.lcd_color_fill(lcd.LcdSide.Right, 255, 0, 0)

def main():

    # Version
    try:
        print(f"[info] LcdControl Version: {lcd.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
        

    # Initialize ToF sensor controler
    rc = lcd.init(lcd.LcdColorDepth.L12BIT)
    if rc != 0:
        print(f"[error] LcdControl.init failed rc={rc}")
        return -2
    
    # brightness 0-10
    lcd.set_brightness(10)

    # read back current depth
    depth = lcd.get_color_depth()
    print(f"[info] Lcd Color Depth: {depth}")

    # Demo 1: fill by buffer
    fill_buffer_example()

    time.sleep(3)

    # Demo 2: fill by color
    fill_color_example()

    # Cleanup
    lcd.dispose()    
    time.sleep(0.2) 

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
