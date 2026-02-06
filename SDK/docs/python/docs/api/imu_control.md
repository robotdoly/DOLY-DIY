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

**Fields**

- **x**
- **y**
- **z**

### `YawPitchRoll`

**Fields**

- **yaw**
- **pitch**
- **roll**

### `ImuData`

**Fields**

- **ypr**
- **linear_accel**
- **temperature**

### `ImuEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onImuUpdate(data: ImuData) -> None`
  - Called when a new IMU sample/update is available.
- `onImuGesture(type: ImuGesture, from: GestureDirection) -> None`
  - Called when a gesture is detected.

## Functions

### `add_listener(listener: ImuEventListener, priority: bool = False) -> None`

Register an ImuEventListener instance.

**Parameters**

- **listener**: Listener object to register.
- **priority**: If `True`, the listener is inserted with higher priority (called earlier). (default: `False`)


### `remove_listener(listener: ImuEventListener) -> None`

Unregister a previously registered ImuEventListener instance.

**Parameters**

- **listener**: Listener object to unregister.


### `on_update(cb: py::function) -> None`

Set a static IMU update callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_gesture(cb: py::function) -> None`

Set a static IMU gesture callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


### `init(delay: int = 0, gx: int = 0, gy: int = 0, gz: int = 0, ax: int = 0, ay: int = 0, az: int = 0) -> int`

Initialize the IMU subsystem.

This must be called once before reading IMU data or receiving events.

**Parameters**

- **delay**: Optional delay (milliseconds) before processing events. (default: `0`)
- **gx**: Gyroscope X offset. (default: `0`)
- **gy**: Gyroscope Y offset. (default: `0`)
- **gz**: Gyroscope Z offset. (default: `0`)
- **ax**: Accelerometer X offset. (default: `0`)
- **ay**: Accelerometer Y offset. (default: `0`)
- **az**: Accelerometer Z offset. (default: `0`)

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already active - -1 : init failed
```

**Notes**

- Sampling rate 104hz (accel + gyro), 1Hz (temperature)


### `dispose() -> int`

Dispose/stop the IMU subsystem and release resources.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not active - -1 : dispose failed
```


### `calculate_offsets(gx: int, gy: int, gz: int, ax: int, ay: int, az: int) -> int`

Calculate and output sensor offsets.

Typically used to perform a calibration routine and retrieve offsets that can be passed to init() on later boots.

**Parameters**

- **gx**: Output gyroscope X offset.
- **gy**: Output gyroscope Y offset.
- **gz**: Output gyroscope Z offset.
- **ax**: Output accelerometer X offset.
- **ay**: Output accelerometer Y offset.
- **az**: Output accelerometer Z offset.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : init failed - -2 : calculation failed
```


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
