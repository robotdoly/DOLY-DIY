"""
example.py

It demonstrates:
- Initializing the edge controller
- Registering the static event listener
- Cleaning up (dispose)

"""


import time
import doly_edge as edge

def on_edge_change(sensors):
    print("[info] Edge change:", [(s.id, s.state) for s in sensors])

def main():

    # Version
    try:
        print(f"[info] EdgeControl Version: {edge.get_version():.3f}")
    except AttributeError:
        pass



    # Initialize EdgeControl
    rc = edge.init()
    if rc < 0:
        print(f"[error] EdgeControl.init failed rc={rc}")
        return -2

    result = 0
    try:
        # Register static event listeners
        edge.on_change(on_edge_change)

        # Run for 20 seconds to test edge detection
        print(f"[info] Run for 20 seconds to test edge detection")
        time.sleep(20)
    finally:
        # Cleanup stays on the control thread; preserve any original exception.
        try:
            cleanup_rc = edge.dispose()
            if cleanup_rc < 0:
                print(f"[error] edge.dispose failed rc={cleanup_rc}")
                result = 1
        except Exception as exc:
            print(f"[error] edge.dispose raised: {exc}")
            result = 1
    time.sleep(0.2)

    return result


if __name__ == "__main__":
    raise SystemExit(main())
