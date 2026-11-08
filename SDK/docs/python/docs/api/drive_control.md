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

The enum members are also exported directly on `doly_drive`:

| Module-level aliases | Enum |
|---|---|
| `Abort`, `Force`, `Motor` | `DriveErrorType` |
| `Both`, `Left`, `Right` | `DriveMotorSide` |
| `Running`, `Completed`, `Error` | `DriveState` |
| `Freestyle`, `XY`, `Distance`, `Rotate` | `DriveType` |

Prefer qualified enum names. `doly_drive.Rotate` refers to `DriveType.Rotate`;
use `DriveErrorType.Rotate` for the error value.

## Classes

### `DriveEventListener`

`DriveEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `on_drive_complete(id: int) -> None`
  - Called when a drive command completes successfully.
- `on_drive_error(id: int, side: DriveMotorSide, type: DriveErrorType) -> None`
  - Called when a drive operation reports an error.
- `on_drive_state_change(drive_type: DriveType, state: DriveState) -> None`
  - Called when the state of a drive operation changes.

The proxy also recognizes legacy Python overrides `onDriveComplete`,
`onDriveError` and `onDriveStateChange`. Override-name selection occurs once at
registration. These aliases are not additional bound base-class methods; prefer
the snake_case methods above.

## Functions

### `add_listener(listener: Optional[DriveEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `DriveEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Repeated registration of the same object is ignored.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: DriveEventListener) -> None`

Unregister the object's registration. **listener** is the Python object previously
passed to `add_listener()`. Removal does not wait for already executing callbacks
to finish.

### `init(imu_off_gx: int = 0, imu_off_gy: int = 0, imu_off_gz: int = 0, imu_off_ax: int = 0, imu_off_ay: int = 0, imu_off_az: int = 0) -> int`

Initialize the drive control module.

IMU offsets are calibration values stored by the platform. They apply only when
the IMU is inactive and at least one supplied gyro offset is nonzero; an
already-active IMU keeps its offsets. See [IMU initialization](imu_control.md)
for the offset conditions and units.

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


### `dispose(dispose_IMU: bool) -> None`

Dispose/shutdown the drive module and release resources.

- **dispose_IMU**: Required boolean, with no default. `True` also requests native
  IMU shutdown; Python IMU registrations are not cleared as a side effect.

Python returns `None`, not the native integer status. Native `0/1` permit Drive
callback cleanup; native `-1` retains surviving registrations. Rejected native
outcomes are reported to stderr. From a wrapper callback, this function returns
`None` immediately without cleanup. A `None` result is not proof of shutdown.

### `is_active() -> bool`

Check whether the module is initialized and active.

**Returns**

- `bool`:

```text
true if active, false otherwise.
```


### `abort() -> None`

Request cancellation of the current drive operation. This call may wait for
previous work to stop; it does not provide an immediate emergency-stop guarantee.
Completion/error events may still be emitted.


### `free_drive(speed: int, is_left: bool, to_forward: bool) -> bool`

Manual motor drive (low-level).

**Parameters**

- **speed**: Motor speed percent (0..100); zero stops the selected motor.
- **is_left**: `True` = left motor, `False` = right motor.
- **to_forward**: `True` = forward, `False` = backward.

**Returns**

- `bool`:

```text
true if accepted.
```

**Notes**

- Directly update the selected motor. Replacing autonomous work may wait for it to stop.


### `go_xy(id: int, x: int, y: int, speed: int, to_forward: bool, with_brake: bool = False, acceleration_interval: int = 0, control_speed: bool = False, control_force: bool = True) -> bool`

Autonomous drive to an (x, y) target (high-level).

**Parameters**

- **id**: User command id (returned in events/callbacks).
- **x**: Target X in the Position coordinate system, in signed integer millimetres.
- **y**: Target Y in the Position coordinate system, in signed integer millimetres.
- **speed**: Requested speed percent; use 1..100. Zero is rejected. Values above 100 are not consistently rejected by the controller and should not be used.
- **to_forward**: Direction preference for the move.
- **with_brake**: If `True`, brake at the end of motion. (default: `False`)
- **acceleration_interval**: Acceleration step interval in milliseconds, an unsigned 8-bit value (0..255); 0 disables acceleration. (default: `0`)
- **control_speed**: Enable speed control dynamically. (default: `False`)
- **control_force**: Enable force/traction control (default `True`). (default: `True`)

**Returns**

- `bool`:

```text
true if command accepted.
```

**Notes**

- Motion runs asynchronously, but submission may wait for previous work. A `True` result means accepted, not completed.


### `go_distance(id: int, mm: int, speed: int, to_forward: bool, with_brake: bool = False, acceleration_interval: int = 0, control_speed: bool = False, control_force: bool = True) -> bool`

Autonomous drive for a fixed distance (high-level).

**Parameters**

- **id**: User command id (returned in events/callbacks).
- **mm**: Distance in millimeters.
- **speed**: Requested speed percent; use 1..100. Zero is rejected. Values above 100 are not consistently rejected by the controller and should not be used.
- **to_forward**: `True` forward / `False` backward.
- **with_brake**: Brake at the end. (default: `False`)
- **acceleration_interval**: Acceleration step interval in milliseconds, an unsigned 8-bit value (0..255); 0 disables acceleration. (default: `0`)
- **control_speed**: Enable speed control dynamically. (default: `False`)
- **control_force**: Enable force/traction control. (default: `True`)

**Returns**

- `bool`:

```text
true if command accepted.
```

**Notes**

- Motion runs asynchronously, but submission may wait for previous work. A `True` result means accepted, not completed.


### `go_rotate(id: int, rotate_angle: float, from_center: bool, speed: int, to_forward: bool, with_brake: bool = False, acceleration_interval: int = 0, control_speed: bool = False, control_force: bool = True) -> bool`

Autonomous rotate (high-level).

**Parameters**

- **id**: User command id (returned in events/callbacks).
- **rotate_angle**: Rotation angle in degrees (sign convention is implementation-defined).
- **from_center**: `True` = rotate around center, `False` = rotate around a wheel/edge (implementation-defined).
- **speed**: Requested speed percent; use 1..100. Zero is rejected. Values above 100 are not consistently rejected by the controller and should not be used.
- **to_forward**: Direction preference (implementation-defined for rotation).
- **with_brake**: Brake at the end. (default: `False`)
- **acceleration_interval**: Acceleration step interval in milliseconds, an unsigned 8-bit value (0..255); 0 disables acceleration. (default: `0`)
- **control_speed**: Enable speed control dynamically. (default: `False`)
- **control_force**: Enable force/traction control. (default: `True`)

**Returns**

- `bool`:

```text
true if command accepted.
```

**Notes**

- Motion runs asynchronously, but submission may wait for previous work. A `True` result means accepted, not completed.


### `get_position() -> doly_helper.Position`

Get current estimated position: `x` and `y` are millimetres and `head` is degrees. See [Position fields](helper.md#position).

**Returns**

- `Position`:

```text
Current Position estimate: the shared doly_helper.Position type.
```


### `reset_position() -> None`

Reset the estimated `x`, `y`, and `head` fields to zero.


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


### `on_complete(cb: Optional[Callable[[int], None]]) -> None`

Set the complete function callback, replacing any previous callable.

- **cb**: Called as `cb(id)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_error(cb: Optional[Callable[[int, DriveMotorSide, DriveErrorType], None]]) -> None`

Set the error function callback, replacing any previous callable.

- **cb**: Called as `cb(id, side, type)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_state_change(cb: Optional[Callable[[DriveType, DriveState], None]]) -> None`

Set the state change function callback, replacing any previous callable.

- **cb**: Called as `cb(drive_type, state)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.


### `get_version() -> float`

Get the current library version, represented as a float.

## Lifecycle contract

`init()` and `abort()` raise `RuntimeError` from any wrapper callback. Perform
control/disposal on the application control thread. `dispose()` keeps its historical
`None` return adapter, including callback rejection; it cannot be interpreted as a
success status. Motion methods return command-acceptance booleans, not proof that
the requested motion completed.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.


