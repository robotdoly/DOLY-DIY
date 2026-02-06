# TofControl API reference

Import:

```python
import doly_tof
```

This page documents the public API exposed by the `doly_tof` Python module.

## Enums

### `TofError`

Values:

- `NoError`
- `VcselContinuityTest`
- `VcselWatchdogTest`
- `VcselWatchdog`
- `Pll1Lock`
- `Pll2Lock`
- `EarlyConvergenceEstimate`
- `MaxConvergence`
- `NoTargetIgnore`
- `MaxSignalToNoiseRatio`
- `RawRangingAlgoUnderflow`
- `RawRangingAlgoOverflow`
- `RangingAlgoUnderflow`
- `RangingAlgoOverflow`
- `FilteredByPostProcessing`
- `DataNotReady`

### `TofSide`

Values:

- `Left`
- `Right`

### `TofGestureType`

Values:

- `Undefined`
- `ObjectComing`
- `ObjectGoing`
- `Scrubing`
- `ToLeft`
- `ToRight`

## Classes

### `TofGesture`

**Fields**

- **type**
- **range_mm**

### `TofData`

**Fields**

- **update_ms**
- **range_mm**
- **error**
- **side**

### `TofEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onProximityGesture(left: TofGesture, right: TofGesture) -> None`
  - Called when a proximity gesture is detected.
- `onProximityThreshold(left: TofData, right: TofData) -> None`
  - Called when proximity threshold is reached (if enabled).

## Functions

### `add_listener(listener: TofEventListener, priority: bool = False) -> None`

Register a TofEventListener instance.

**Parameters**

- **listener**: Listener object to register.
- **priority**: If `True`, the listener is inserted with higher priority (called earlier). (default: `False`)


### `remove_listener(listener: TofEventListener) -> None`

Unregister a previously registered TofEventListener instance.

**Parameters**

- **listener**: Listener object to unregister.


### `on_proximity_gesture(cb: py::function) -> None`

Set a static proximity-gesture callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_proximity_threshold(cb: py::function) -> None`

Set a static proximity-threshold callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


### `init(offset_left: int = 0, offset_right: int = 0) -> int`

Initialize ToF sensors.

**Parameters**

- **offset_left**: Offset to apply to left sensor readings. (default: `0`)
- **offset_right**: Offset to apply to right sensor readings. (default: `0`)

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already running - -1 : both sensor init failed - -2 : left sensor init failed - -3 : right sensor init failed
```


### `dispose() -> int`

Dispose/stop the ToF subsystem and release resources.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not initialized
```


### `setup_continuous(interval_ms: int = 50, distance: int = 0) -> int`

Configure continuous reading for gesture detection and threshold events.

**Parameters**

- **interval_ms**: Read interval in milliseconds (max = 2550 ms). Use 0 to stop continuous reading. (default: `50`)
- **distance**: Proximity threshold distance in millimeters: - 0 disables proximity threshold events - 120 mm is the maximum supported threshold (default: `0`)

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already running - -1 : interval_ms out of range (max = 2550) - -2 : ToF devices not ready - -3 : proximity distance out of range (0..120 mm)
```

**Notes**

- For better gesture detection, recommended interval is 30..80 ms (ideal: 50 ms).


### `get_sensors_data() -> list[TofData]`

Read both sensors once and return the latest values.

Continuous setup is not required for single reads.

**Returns**

- `list[TofData]`:

```text
Vector containing left and right sensor data.
```


### `is_active() -> bool`

Check whether the ToF subsystem is active (initialized).

**Returns**

- `bool`:

```text
true if initialized; false otherwise.
```


### `is_reading() -> bool`

Check whether the subsystem is currently performing frequent/continuous readings.

**Returns**

- `bool`:

```text
true if continuous reading is active; false otherwise.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
