# IoControl API reference

Import:

```python
import doly_io
```

This page documents the public API exposed by the `doly_io` Python module.

## Enums

### `GpioState`

Values:

- `Low`
- `High`

## Functions

### `write_pin(id: int, io_pin: int, state: GpioState) -> int`

Write a GPIO state to an IO port pin.

**Parameters**

- **id**: User-defined identifier used for internal tracking/logging (unsigned 16-bit, 0..65535).
- **io_pin**: IO port pin number (valid range: 0..5).
- **state**: Desired GPIO output state.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : invalid GPIO pin (must be IO port pin number 0..5) - -2 : GPIO write error
```


### `read_pin(id: int, io_pin: int) -> GpioState`

Read the current GPIO state of an IO port pin.

**Parameters**

- **id**: User-defined identifier used for internal tracking/logging (unsigned 16-bit, 0..65535).
- **io_pin**: IO port pin number (valid range: 0..5).

**Returns**

- `GpioState`:

```text
Current GPIO state. Native invalid-pin, missing-pin, invalid-descriptor and
read-failure paths return GpioState.Low, indistinguishable from a valid LOW input.
There is no separate error status.
```


Physical pins 0..5 are supported. Native pin values 6..255 return `GpioState.Low`;
values outside the unsigned 8-bit range fail Python conversion before the read.

### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

