"""
example.py

It demonstrates:
- Initializing the battery controller
- Registering the static event listener
- Reading the battery information
- Cleaning up

"""


import time
import doly_battery as battery
import doly_helper as helper


def on_alarm(capacity:int):
    print(f"[info] Battery Alarm! capacity={capacity}")


def main():
    # Version
    try:
        print(f"[info] BatteryControl Version: {battery.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1

    # Register static event listeners 
    battery.on_alarm(on_alarm)

    # Initialize BatteryControl
    rc = battery.init()
    if rc != 0:
        print("[error] BatteryControl init failed")
        return -2

    # Get battery capacity after a short delay following initialization.
    time.sleep(0.5)
    val = battery.get_capacity()
    print(f"[info] Battery capacity={val}")

    # Set alarm threshold to 20% 
    battery.set_alarm_threshold(20)

    # Wait alarm event
    print("[info] Wait for alarm event for 15 seconds")
    time.sleep(15)

    # Cleanup
    battery.dispose()   

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
