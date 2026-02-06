# DriveControl API reference

Import:

```python
import doly_drive
```

This page documents the public API exposed by the `doly_drive` Python module.

## Enums

### `DriveErrorType`

Values:

- `Abort`
- `Force`
- `Rotate`
- `Motor`

### `DriveMotorSide`

Values:

- `Both`
- `Left`
- `Right`

### `DriveState`

Values:

- `Running`
- `Completed`
- `Error`

### `DriveType`

Values:

- `Freestyle`
- `XY`
- `Distance`
- `Rotate`

## Classes

### `DriveEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `on_drive_complete(id: int) -> None`
  - Called when a drive command completes successfully.
- `on_drive_error(id: int, side: DriveMotorSide, type: DriveErrorType) -> None`
  - Called when a drive operation reports an error.
- `on_drive_state_change(drive_type: DriveType, state: DriveState) -> None`
  - Called when the state of a drive operation changes.

## Functions

### `add_listener(listener_obj: DriveEventListener) -> None`

Register a listener object to receive drive events.

**Parameters**

- **listener_obj**: Pointer to a listener instance (must not be null).

**Notes**

- Callbacks are typically invoked from an internal worker/event thread.


### `remove_listener(listener_obj: DriveEventListener) -> None`

Unregister a listener object.

**Parameters**

- **listener_obj**: Pointer previously passed to AddListener().


### `init(imu_off_gx: int = 0, imu_off_gy: int = 0, imu_off_gz: int = 0, imu_off_ax: int = 0, imu_off_ay: int = 0, imu_off_az: int = 0) -> int`

Initialize the drive control module.

IMU offsets are calibration values stored by the platform.

**Parameters**

- **imu_off_gx**: IMU gyro X offset (calibration). (default: `0`)
- **imu_off_gy**: IMU gyro Y offset (calibration). (default: `0`)
- **imu_off_gz**: IMU gyro Z offset (calibration). (default: `0`)
- **imu_off_ax**: IMU accel X offset (calibration). (default: `0`)
- **imu_off_ay**: IMU accel Y offset (calibration). (default: `0`)
- **imu_off_az**: IMU accel Z offset (calibration). (default: `0`)

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already active / already initialized - -1 : motor setup failed - -2 : IMU init failed
```


### `dispose(dispose_IMU: bool) -> int`

Dispose/shutdown drive module and release resources. Removes the call back functions.

**Parameters**

- **dispose_IMU**: If `True`, shuts down the IMU module as well.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not active / not initialized
```


### `is_active() -> bool`

Check whether the module is initialized and active.

**Returns**

- `bool`:

```text
true if active, false otherwise.
```


### `abort() -> None`

Abort current drive operation immediately.

Typically used as an emergency stop for autonomous movement. Completion/error events may still be emitted depending on implementation.


### `free_drive(speed: int, is_left: bool, to_forward: bool) -> bool`

Manual motor drive (low-level).

**Parameters**

- **speed**: Motor speed percent (0..100).
- **is_left**: `True` = left motor, `False` = right motor.
- **to_forward**: `True` = forward, `False` = backward.

**Returns**

- `bool`:

```text
true if accepted.
```

**Notes**

- Non-blocking; handled in another thread.


### `go_xy(id: int, x: int, y: int, speed: int, to_forward: bool, with_brake: bool = False, acceleration_interval: int = 0, control_speed: bool = False, control_force: bool = True) -> bool`

Autonomous drive to an (x, y) target (high-level).

**Parameters**

- **id**: User command id (returned in events/callbacks).
- **x**: Target X in your Position coordinate system (units are application-defined).
- **y**: Target Y in your Position coordinate system (same unit as x).
- **speed**: Requested speed percent (0..100).
- **to_forward**: Direction preference for the move.
- **with_brake**: If `True`, brake at the end of motion. (default: `False`)
- **acceleration_interval**: Acceleration step interval (0 = disabled). (default: `0`)
- **control_speed**: Enable speed control dynamically. (default: `False`)
- **control_force**: Enable force/traction control (default `True`). (default: `True`)

**Returns**

- `bool`:

```text
true if command accepted.
```

**Notes**

- Non-blocking; handled in another thread.


### `go_distance(id: int, mm: int, speed: int, to_forward: bool, with_brake: bool = False, acceleration_interval: int = 0, control_speed: bool = False, control_force: bool = True) -> bool`

Autonomous drive for a fixed distance (high-level).

**Parameters**

- **id**: User command id (returned in events/callbacks).
- **mm**: Distance value (named "mm" in API; ensure your implementation uses millimeters).
- **speed**: Requested speed percent (0..100).
- **to_forward**: `True` forward / `False` backward.
- **with_brake**: Brake at the end. (default: `False`)
- **acceleration_interval**: Acceleration step interval (0 = disabled). (default: `0`)
- **control_speed**: Enable speed control dynamically. (default: `False`)
- **control_force**: Enable force/traction control. (default: `True`)

**Returns**

- `bool`:

```text
true if command accepted.
```

**Notes**

- Non-blocking; handled in another thread.


### `go_rotate(id: int, rotate_angle: float, from_center: bool, speed: int, to_forward: bool, with_brake: bool = False, acceleration_interval: int = 0, control_speed: bool = False, control_force: bool = True) -> bool`

Autonomous rotate (high-level).

**Parameters**

- **id**: User command id (returned in events/callbacks).
- **rotate_angle**: Rotation angle in degrees (sign convention is implementation-defined).
- **from_center**: `True` = rotate around center, `False` = rotate around a wheel/edge (implementation-defined).
- **speed**: Requested speed percent (0..100).
- **to_forward**: Direction preference (implementation-defined for rotation).
- **with_brake**: Brake at the end. (default: `False`)
- **acceleration_interval**: Acceleration step interval (0 = disabled). (default: `0`)
- **control_speed**: Enable speed control dynamically. (default: `False`)
- **control_force**: Enable force/traction control. (default: `True`)

**Returns**

- `bool`:

```text
true if command accepted.
```

**Notes**

- Non-blocking; handled in another thread.


### `get_position() -> Position`

Get current estimated position.

**Returns**

- `Position`:

```text
Current Position estimate (see Helper.h for definition/units).
```


### `reset_position() -> None`

Reset current position estimate to (0, 0, 0) (implementation-defined fields).


### `get_state() -> DriveState`

Get current drive state.

**Returns**

- `DriveState`:

```text
Current DriveState.
```


### `get_rpm(is_left: bool) -> float`

Get current motor RPM.

**Parameters**

- **is_left**: `True` = left motor, `False` = right motor.

**Returns**

- `float`:

```text
RPM value (units: revolutions per minute).
```


### `on_complete(cb_obj: void(onComplete)(uint16_t) -> None`

Remove a previously registered “command complete” callback.

**Parameters**

- **cb_obj**: Same function pointer passed to AddListenerOnComplete().


### `on_error(cb_obj: void(onError)(uint16_t id, DriveMotorSide side, DriveErrorType) -> None`

Remove a previously registered error callback.

**Parameters**

- **cb_obj**: Same function pointer passed to AddListenerOnError().


### `on_state_change(cb_obj: void(onChange)(DriveType drive_type, DriveState) -> None`

Remove a previously registered state-change callback.

**Parameters**

- **cb_obj**: Same function pointer passed to AddListenerOnStateChange().


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.
