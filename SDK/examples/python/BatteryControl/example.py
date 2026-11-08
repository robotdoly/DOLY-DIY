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


def on_alarm(capacity:int):
    print(f"[info] Battery Alarm! capacity={capacity}")


def main():
    # Version
    try:
        print(f"[info] BatteryControl Version: {battery.get_version():.3f}")
    except AttributeError:
        pass


    result = 0
    initialized = False
    registered = []
    try:
        # Register static event listeners
        battery.on_alarm(on_alarm)
        registered.append(battery.on_alarm)

        # Initialize BatteryControl
        rc = battery.init()
        if rc < 0:
            print("[error] BatteryControl init failed")
            return -2
        initialized = True

        # Get battery capacity after a short delay following initialization.
        time.sleep(0.5)
        val = battery.get_capacity()
        print(f"[info] Battery capacity={val}")

        # Set alarm threshold to 100%, will be trigger at %99
        battery.set_alarm_threshold(100)

        # Wait alarm event
        print("[info] Wait for alarm event for 15 seconds")
        time.sleep(15)
        
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
        try:
            cleanup_rc = battery.dispose()
            if cleanup_rc < 0:
                print(f"[error] battery.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] battery.dispose raised: {exc}")
            result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
