# EdgeControl (Python)



## Import

```python
import doly_helper
```

## Minimal working example

Below is the module’s `example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Initializing the edge controller
- Registering the static event listener
- Cleaning up (remove_listener + dispose)

"""


import time
import doly_helper as helper
import doly_edge as edge

def on_edge_change(sensors):
    print("[info] Edge change:", [(s.id, s.state) for s in sensors])

def main():

    # Version
    try:
        print(f"[info] EdgeControl Version: {edge.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
        

    # Initialize ArmControl
    rc = edge.init()
    if rc < 0:
        print(f"[error] EdgeControl.init failed rc={rc}")
        return -2  

    # Register static event listeners 
    edge.on_change(on_edge_change)    

    # Run for 20 seconds to test edge detection
    print(f"[info] Run for 20 seconds to test edge detection")
    time.sleep(20) 

    # Cleanup     
    edge.dispose()       
    time.sleep(0.2) 

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Initialize**
- **Add event listener**
- **Read state / values**
- **Dispose / cleanup**

## Notes

- **Platform:** Raspberry Pi OS
- **Python:** 3.11
- **Installed on robot:** Yes (preinstalled)

## API reference

See: [API reference](../api/edge_control.md)
