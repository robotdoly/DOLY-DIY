# IoControl (Python)

## Import

```python
import doly_io
```

## Minimal working example

Below is the module’s `examples/python/IoControl/example.py` as a starting point.

```python
"""
example.py

It demonstrates:
- Writing IO pin state
- Reading IO pin state

"""

import doly_io as io

def main():

    # Version
    try:
        print(f"[info] IoControl Version: {io.get_version():.3f}")
    except AttributeError:
        pass


    # INFORMATION
    print("[info] Before testing, connect IO port 0 and port 1 (pin_0 <-> pin_1).")

    result = 0
    output_attempted = False
    low_confirmed = False
    try:
        # write pin state => HIGH
        io_0 = 0 # IO port pin_0
        output_attempted = True
        rc = io.write_pin(1, io_0, io.GpioState.High)
        if rc < 0:
            print(f"[error] Write HIGH failed rc={rc}")
            return -1
        # read IO pin state
        io_1 = 1 # IO port pin_1
        state = io.read_pin(2, io_1)
        print(f"[info] Read Pin:{io_1} State:{state}")

        # write pin state => LOW
        rc = io.write_pin(1, io_0, io.GpioState.Low)
        if rc < 0:
            print(f"[error] Write LOW failed rc={rc}")
            return -1
        low_confirmed = True
        # read IO pin state
        state = io.read_pin(2, io_1)
        print(f"[info] Read Pin:{io_1} State:{state}")
    finally:
        if output_attempted and not low_confirmed:
            try:
                cleanup_rc = io.write_pin(1, io_0, io.GpioState.Low)
                if cleanup_rc < 0:
                    print(f"[error] Final IO0 LOW failed rc={cleanup_rc}")
                    result = 1
            except Exception as exc:
                print(f"[error] Final IO0 LOW raised: {exc}")
                result = 1

    return result


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Stop Doly service**
- **Read/Write pin state**

## API reference

See: [API reference](../api/io_control.md)

