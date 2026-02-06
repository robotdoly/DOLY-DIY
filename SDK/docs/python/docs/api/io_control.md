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

- **id**: User-defined identifier used for internal tracking/logging (can be any value).
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

- **id**: User-defined identifier used for internal tracking/logging (can be any value).
- **io_pin**: IO port pin number (valid range: 0..5).

**Returns**

- `GpioState`:

```text
Current GPIO state. If @p io_pin is invalid or a read error occurs, the returned value is implementation-defined (see underlying GPIO layer).
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
