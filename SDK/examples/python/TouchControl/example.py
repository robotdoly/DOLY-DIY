"""
example.py

It demonstrates:
- Initializing the touch controller
- Registering the static event listeners
- Cleaning up (remove_listener + dispose)

"""


import time
import doly_helper as helper
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

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
        

    # Initialize TouchControl
    rc = touch.init()
    if rc < 0:
        print(f"[error] TouchControl.init failed rc={rc}")
        return -2  

    # Register static event listeners 
    touch.on_touch(on_touch)
    touch.on_touch_activity(on_touch_activity)    

    # Run for 30 seconds to test touch events
    print(f"[info] Run for 30 seconds to test touch events")
    time.sleep(30) 

    # Cleanup     
    touch.dispose()       
    time.sleep(0.2) 

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
