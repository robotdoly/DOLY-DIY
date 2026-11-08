# FanControl API reference

Import:

```python
import doly_fan
```

This page documents the public API exposed by the `doly_fan` Python module.

## Functions

### `init(auto_control: bool) -> int`

Initialize the Fan Control module.

**Parameters**

- **auto_control**: Required argument, with no default. If `True`, automatic
  temperature control will manage fan speed.

**Returns**

- `int`:

```text
0: success
1: already initialized
-1: temperature source unavailable
-2: fan device open failed
```


### `dispose() -> int`

Dispose the Fan Control module and release resources.

**Returns**

- `int`:

```text
0: success
1: not initialized
-1: temperature worker cancellation failed
```


### `set_fan_speed(percentage: int) -> int`

Set fan speed as a percentage. The Python adapter clamps signed integer input
to 0..100 before forwarding it to the native controller.

**Parameters**

- **percentage**: Fan speed percentage (0..100).

**Returns**

- `int`:

```text
0: native ioctl reported success
<0: native error
```

**Notes**

- If `auto_control` was enabled in `init()`, the automatic controller may override
  or adjust the fan speed over time.


### `is_active() -> bool`

Check whether the Fan Control module is active (initialized).

**Returns**

- `bool`:

```text
true if initialized; false otherwise.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

## Lifecycle restrictions

`init()` and `dispose()` raise `RuntimeError` from any wrapper callback. Call them
on the application control thread. Native status errors remain integer results.


