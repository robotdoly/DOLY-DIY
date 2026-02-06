# Helper (Python)



## Import

```python
import doly_helper
```

## Minimal working example

Below is the module’s `example.py` as a starting point.

```python
import doly_helper as helper

def main():
    # Read settings
    res = helper.read_settings()
    if res < 0:
        print(f"[info] Read settings failed with code: {res}")
        return 1

    # Get IMU offsets
    res, gx, gy, gz, ax, ay, az = helper.get_imu_offsets()
    if res < 0:
        print(f"[info] Get IMU offsets failed with code: {res}")
        return 1

    print(f"[info] IMU Offsets - Gx:{gx} Gy:{gy} Gz:{gz} Ax:{ax} Ay:{ay} Az:{az}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Read settings**

## Notes

- **Platform:** Raspberry Pi OS
- **Python:** 3.11
- **Installed on robot:** Yes (preinstalled)

## API reference

See: [API reference](../api/helper.md)
