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

`TofGesture()` creates a record. Fields are writable.

**Fields**

- **type: TofGestureType**: Gesture classification.
- **range_mm: int**: Range in millimeters, initially 0.

### `TofData`

`TofData()` creates a record. Fields are writable.

**Fields**

- **update_ms: int**: Update timestamp in milliseconds, initially 0.
- **range_mm: int**: Range in millimeters, initially 0.
- **error: TofError**: Sample error, initially `TofError.NoError`.
- **side: TofSide**: Sensor side.

### `TofEventListener`

`TofEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onProximityGesture(left: TofGesture, right: TofGesture) -> None`
  - Called when a proximity gesture is detected.
- `onProximityThreshold(left: TofData, right: TofData) -> None`
  - Called when proximity threshold is reached (if enabled).

## Functions

### `add_listener(listener: Optional[TofEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `TofEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: TofEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_proximity_gesture(cb: Optional[Callable[[TofGesture, TofGesture], None]]) -> None`

Set the proximity gesture function callback, replacing any previous callable.

- **cb**: Called as `cb(left, right)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_proximity_threshold(cb: Optional[Callable[[TofData, TofData], None]]) -> None`

Set the proximity threshold function callback, replacing any previous callable.

- **cb**: Called as `cb(left, right)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `init(offset_left: int = 0, offset_right: int = 0) -> int`

Initialize ToF sensors.

**Parameters**

- **offset_left**: Offset to apply to left sensor readings. (default: `0`)
- **offset_right**: Offset to apply to right sensor readings. (default: `0`)

**Returns**

- `int`:

```text
0: success
1: already initialized / lifecycle in progress
-1: setup, initial resource or right-sensor initialization failure
-2: left-sensor enable/setup failure
```


### `dispose() -> int`

Dispose/stop the ToF subsystem and release resources.

**Returns**

- `int`:

```text
0: disposal accepted, potentially deferred or ongoing
1: already inactive or cleanup already completed
-1: post-drain release error; Python callback cleanup accepted
-2: disposal rejected; registrations retained
```


### `setup_continuous(interval_ms: int = 50, distance: int = 0) -> int`

Configure continuous reading for gesture detection and threshold events.

**Parameters**

- **interval_ms**: Read interval in milliseconds (max = 2550 ms). Use 0 to stop continuous reading. (default: `50`)
- **distance**: Proximity threshold distance in millimeters: - 0 disables proximity threshold events - 120 mm is the maximum supported threshold (default: `0`)

**Returns**

- `int`:

```text
0: success
1: already running
-1: interval_ms exceeds 2550
-2: distance exceeds 120 mm
-3: devices not ready
-4: worker stop/start failure
```

**Notes**

- For better gesture detection, recommended interval is 30..80 ms (ideal: 50 ms).


### `get_sensors_data() -> list[TofData]`

Read both sensors in single-read mode, or wait for fresh measurements from both
sensors in continuous mode. Continuous setup is not required for single reads.
The list can be empty if sensors are unavailable or fresh measurements cannot be
obtained before continuous polling stops or times out. Inspect each record's
`error` field before using its range.

**Returns**

- `list[TofData]`:

```text
Left sensor data followed by right sensor data, or an empty list.
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

## Lifecycle contract

`init()`, `setup_continuous()` and `dispose()` raise `RuntimeError` from any
wrapper callback. Disposal outcomes `0/1/-1` accept Python callback cleanup,
including ongoing/deferred disposal; they do not universally prove native worker
drainage. `-2` retains registrations.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.


