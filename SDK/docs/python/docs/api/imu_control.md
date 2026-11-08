# ImuControl API reference

Import:

```python
import doly_imu
```

This page documents the public API exposed by the `doly_imu` Python module.

## Enums

### `ImuGesture`

Values:

- `Undefined`
- `Move`
- `LongShake`
- `ShortShake`
- `Vibrate`
- `VibrateExtreme`
- `ShockLight`
- `ShockMedium`
- `ShockHard`
- `ShockExtreme`

### `GestureDirection`

Values:

- `Left`
- `Right`
- `Up`
- `Down`
- `Front`
- `Back`

## Classes

### `VectorFloat`

`VectorFloat()` creates a record. Fields are writable.

**Fields**

- **x: float**
- **y: float**
- **z: float**

### `YawPitchRoll`

`YawPitchRoll()` creates a record. Fields are writable.

**Fields**

- **yaw: float**
- **pitch: float**
- **roll: float**

### `ImuData`

`ImuData()` creates a record. Fields are writable.

**Fields**

- **ypr: YawPitchRoll**
- **linear_accel: VectorFloat**
- **temperature: float**

### `ImuEventListener`

`ImuEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onImuUpdate(data: ImuData) -> None`
  - Called when a new IMU sample/update is available.
- `onImuGesture(type: ImuGesture, direction: GestureDirection) -> None`
  - Called when a gesture is detected.

## Functions

### `add_listener(listener: Optional[ImuEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `ImuEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: ImuEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_update(cb: Optional[Callable[[ImuData], None]]) -> None`

Set the update function callback, replacing any previous callable.

- **cb**: Called as `cb(data)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_gesture(cb: Optional[Callable[[ImuGesture, GestureDirection], None]]) -> None`

Set the gesture function callback, replacing any previous callable.

- **cb**: Called as `cb(type, direction)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `init(delay: int = 0, gx: int = 0, gy: int = 0, gz: int = 0, ax: int = 0, ay: int = 0, az: int = 0) -> int`

Initialize the IMU subsystem.

This must be called once before reading IMU data or receiving events.

**Parameters**

- **delay**: Startup delay in seconds before normal motion processing. (default: `0`)
- **gx**: Gyroscope X offset in millidegrees per second (mdps). (default: `0`)
- **gy**: Gyroscope Y offset in millidegrees per second (mdps). (default: `0`)
- **gz**: Gyroscope Z offset in millidegrees per second (mdps). (default: `0`)
- **ax**: Accelerometer X offset in milligravity units (mg, 1/1000 g). (default: `0`)
- **ay**: Accelerometer Y offset in milligravity units (mg, 1/1000 g). (default: `0`)
- **az**: Accelerometer Z offset in milligravity units (mg, 1/1000 g). (default: `0`)

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already active - -1 : init failed
```

**Notes**

- Nominal sampling rate is 104 Hz for accelerometer/gyro and 1 Hz for temperature;
  this is not a guarantee of Python callback delivery rate.
- `delay` is measured in seconds.
- All six supplied offsets apply only when initializing an inactive IMU and at
  least one of `gx`, `gy`, or `gz` is nonzero. Nonzero accelerometer offsets
  alone are ignored by this initialization path. An already-active result of
  `1` does not replace offsets.


### `dispose() -> int`

Dispose/stop the IMU subsystem and release resources.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not active - -1 : dispose failed
```


### `calculate_offsets() -> tuple[int, int, int, int, int, int, int]`

Calculate sensor offsets using a blocking hardware calibration routine. This
function takes no arguments. The offsets can be passed to `init()` on later boots.

**Returns**

`(result, gx, gy, gz, ax, ay, az)`: status followed by gyro X/Y/Z and accelerometer
X/Y/Z offsets. `result` is `0` on success, `-1` for initialization failure, or
`-2` for calculation failure. Check the status before using offsets; failure
placeholders are not valid calibration. In a wrapper callback this raises
`RuntimeError` instead of entering blocking native work.

### `get_imu_data() -> ImuData`

Get the last IMU reading snapshot.

**Returns**

- `ImuData`:

```text
Latest ImuData captured by the IMU subsystem.
```

**Notes**

- init() must have been called successfully before meaningful data is available.


### `get_temperature() -> float`

Get the last temperature reading from the IMU.

**Returns**

- `float`:

```text
Temperature in degrees Celsius.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

### `get_gesture_str(type: ImuGesture) -> str`

Return the native display string for the gesture enum.

### `get_direction_str(from: GestureDirection) -> str`

Return the native display string for the direction enum. The binding argument
name is `from`, which is a Python keyword. Call positionally, for example
`doly_imu.get_direction_str(doly_imu.GestureDirection.Left)`; do not write `from=...`.

## Lifecycle contract

`init()` and `calculate_offsets()` raise `RuntimeError` from any wrapper
callback. `dispose()` instead returns `-1` there. Native disposal `0/1` accepts
callback cleanup; `-1` may represent callback rejection or resource failure, so
the wrapper conservatively retains registrations. Perform cleanup on the control
thread. `YawPitchRoll` angles are degrees and temperature is degrees Celsius;
linear acceleration uses milligravity units (mg, 1/1000 g). These units describe
the numeric scaling, not calibration accuracy.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.


