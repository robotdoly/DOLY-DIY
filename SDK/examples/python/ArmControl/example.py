"""
example.py

This example shows how to implement an event listener in Python
(similar to `class MyListener : public ArmEventListener` in C++) and use it
with the ArmControl API.

It demonstrates:
- Initializing the arm controller
- Creating a listener class by inheriting from ArmEventListener
- Registering the listener instance (non-static / object-based)
- Sending a movement command using set_angle()
- Reading back the current angle(s)
- Cleaning up (remove_listener + dispose)

Important notes about python wrapper:
- You MUST keep a Python reference to the listener object while it is registered.
  If the listener is garbage-collected, the C++ side may still hold a pointer to it,
  which can cause a crash.
"""


import time
import doly_arm as arm
import doly_helper as helper


class MyListener(arm.ArmEventListener):
    def onArmComplete(self, id, side):
        print("[info] complete", id, side)

    def onArmError(self, id, side, errorType):
        print("[error] error", id, side, errorType)

    def onArmStateChange(self, side, state):
        print("[info] state", side, state)

    def onArmMovement(self, side, degreeChange):
        print("[info] move", side, degreeChange)


def main():

    # Version
    try:
        print(f"[info] ArmControl Version: {arm.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
    
    # *** IMPORTANT *** 
    # Read settings for servo calibration data, otherwise arm control won't work.
    # one time read is sufficient for the lifetime of the application	
    if helper.read_settings() < 0: 
        print("[error] Read settings failed")
        return -2

    # Initialize ArmControl
    rc = arm.init()
    if rc < 0:
        print(f"[error] ArmControl.init failed rc={rc}")
        return -2  

    # add event listener
    listener = MyListener()
    arm.add_listener(listener, priority=False)


    # Example command
    cmd_id = 1
    rc = arm.set_angle(cmd_id, arm.ArmSide.Both, speed=30, angle=90, with_brake=False)
    if rc < 0:
        print(f"[error] set_angle failed rc={rc}")
        return -3

    # Wait until done (simple polling example)
    while arm.get_state(arm.ArmSide.Both) != arm.ArmState.Completed:
        time.sleep(0.05)

    angles = arm.get_current_angle(arm.ArmSide.Both)
    for a in angles:
        print(f"angle: side={a.side} angle={a.angle}")    

    # Cleanup     
    arm.dispose()       
    time.sleep(0.2)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
