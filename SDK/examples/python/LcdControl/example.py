"""
example.py

It demonstrates:
- Initializing the LCD controller
- Writes a full white frame to both displays
- Fills left display blue and right display red
- Cleaning up (dispose)

"""

import time
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



    # Initialize LCD controller (exclusive access; do not run alongside Eye)
    rc = lcd.init(lcd.LcdColorDepth.L12BIT)
    if rc < 0:
        print(f"[error] LcdControl.init failed rc={rc}")
        return -2

    result = 0
    try:
        # brightness 0-10
        rc = lcd.set_brightness(10)
        if rc < 0:
            print(f"[error] Set brightness failed rc={rc}")
            return -3

        # read back current depth
        depth = lcd.get_color_depth()
        print(f"[info] Lcd Color Depth: {depth}")

        if depth != lcd.LcdColorDepth.L12BIT:
            print("[error] LCD is not in the requested 12-bit mode")
            return -3

        # Demo 1: fill by buffer
        if fill_buffer_example() < 0:
            return -3

        time.sleep(3)

        # Demo 2: fill by color
        fill_color_example()
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
        try:
            cleanup_rc = lcd.dispose()
            if cleanup_rc < 0:
                print(f"[error] lcd.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] lcd.dispose raised: {exc}")
            result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
