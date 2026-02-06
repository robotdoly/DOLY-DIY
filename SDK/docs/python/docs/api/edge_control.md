# EdgeControl API reference

Import:

```python
import doly_edge
```

This page documents the public API exposed by the `doly_edge` Python module.

## Enums

### `GpioState`

Values:

- `Low`
- `High`

### `GapDirection`

Values:

- `Front`
- `Front_Left`
- `Front_Right`
- `Back`
- `Back_Left`
- `Back_Right`
- `Left`
- `Right`
- `Cross_Left`
- `Cross_Right`
- `All`

### `SensorId`

Values:

- `Back_Left`
- `Back_Right`
- `Front_Left`
- `Front_Right`

## Classes

### `IrSensor`

**Fields**

- **id**
- **state**

### `EdgeEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onEdgeChange(sensors: list[IrSensor]) -> None`
  - Called when the set of IR sensor states changes.
- `onGapDetect(gap_type: GapDirection) -> None`
  - Called when a gap (e.g. drop-off, missing surface) is detected and categorized.

## Functions

### `add_listener(listener: EdgeEventListener, priority: bool = False) -> None`

Register an EdgeEventListener instance.

**Parameters**

- **listener**: Listener object pointer.
- **priority**: If `True`, the listener may be inserted with higher priority (implementation-defined ordering). (default: `False`)


### `remove_listener(listener: EdgeEventListener) -> None`

Unregister an EdgeEventListener instance.

**Parameters**

- **listener**: Listener object pointer previously registered.


### `clear_listeners() -> None`

Unregister all static callbacks and class-based listeners tracked by the binding.


### `on_change(cb: py::function) -> None`

Set a static on-change callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_gap_detect(cb: py::function) -> None`

Set a static gap-detect callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `init() -> int`

Initialize edge sensor control.

Must be called before enabling control or reading sensors.

**Returns**

- `int`:

```text
Status code: - 0 : success - <0 : error (implementation-defined)
```


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


### `dispose() -> int`

Dispose edge sensor control and release resources. Removes the call back functions.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not initialized - -1..-6 : GPIO release failed (specific code indicates which GPIO failed)
```


### `is_active() -> bool`

Check whether edge sensor control is initialized and active.

**Returns**

- `bool`:

```text
true if active, false otherwise.
```


### `enable_control() -> int`

Start the sensor listening thread.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already running - -1 : not initialized
```

**Notes**

- The initialize function enables the module on startup.


### `disable_control() -> int`

Stop the sensor listening thread.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not running - -1 : not initialized
```


### `get_sensors(state: GpioState) -> list[IrSensor]`

Get IR sensors filtered by GPIO state.

**Parameters**

- **state**: Desired GPIO state to filter by. 0 = no ground detected (free) 1 = ground detected

**Returns**

- `list[IrSensor]`:

```text
Vector containing sensors matching the specified state.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
