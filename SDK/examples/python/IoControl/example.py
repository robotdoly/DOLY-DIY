"""
example.py

It demonstrates:
- Writing IO pin state
- Reading IO pin state

"""

import time
import doly_helper as helper
import doly_io as io

def main():

    # Version
    try:
        print(f"[info] IoControl Version: {io.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
    
    # INFORMATION
    print("[info] Before testing, connect IO port 0 and port 1 (pin_0 <-> pin_1).")

    # write pin state => HIGH 
    io_0 = 0 # IO port pin_0
    io.write_pin(1, 0, io.GpioState.High)
    # read IO pin state
    io_1 = 1 # IO port pin_1
    state = io.read_pin(2, io_1);
    print(f"[info] Read Pin:{io_1} State:{state}")

    # write pin state => LOW 
    io.write_pin(1, 0, io.GpioState.Low)
    # read IO pin state
    state = io.read_pin(2, io_1);
    print(f"[info] Read Pin:{io_1} State:{state}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
