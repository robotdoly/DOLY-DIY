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

- **auto_control**: If `True`, automatic temperature control will manage fan speed.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized - <0 : error (implementation-defined)
```


### `dispose() -> int`

Dispose the Fan Control module and release resources.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not initialized - <0 : error
```


### `set_fan_speed(percentage: int) -> int`

Set fan speed as a percentage.

**Parameters**

- **percentage**: Fan speed percentage (0..100).

**Returns**

- `int`:

```text
Status code: - <0 : error
```

**Notes**

- If @c auto_control was enabled in init(), the automatic controller may override
- or adjust the fan speed over time.


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
