"""
example.py

It demonstrates:
- Initializing the ToF sensor controller
- Registering the static event listeners
- Cleaning up (remove_listener + dispose)

"""


import time
import doly_helper as helper
import doly_tof as tof

# Event returns TofGesture data for both sides.
def on_proximity_gesture(left, right):
    print(f"[info] Gesture event L={left.type} R={right.type}")

# Event returns TofData information for both sides.
def on_proximity_threshold(left, right):
    print(f"[info] Proximity event range L={left.range_mm} R={right.range_mm}")

def main():

    # Version
    try:
        print(f"[info] TofControl Version: {tof.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
        

    # Initialize ToF sensor controler
    rc = tof.init()
    if rc < 0:
        print(f"[error] TofControl.init failed rc={rc}")
        return -2
    
    # Setup continues ToF control with 50ms interval and proximity threshold 40mm (enabled)
    if tof.setup_continuous(50, 40) < 0:    
        print("[error] TofControl setup failed")
        return -3;    

    # Register static event listeners 
    tof.on_proximity_gesture(on_proximity_gesture)
    tof.on_proximity_threshold(on_proximity_threshold)    

    # Run for 20 seconds to test TOF events
    print(f"[info] Run for 20 seconds to test TOF sensor events")
    time.sleep(20) 

    # Stop continuous reading and disable events
    if tof.setup_continuous(0) < 0:    
        print("[error] TofControl setup failed")
        return -4;  

    # Single read example
    sensors = tof.get_sensors_data(); 
    for sensor in sensors:
        print(f"[info] Single Read Sensor[{sensor.side}] Range[{sensor.range_mm}mm] Error[{sensor.error}]")

    # Cleanup     
    tof.dispose()       
    time.sleep(0.2) 

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
