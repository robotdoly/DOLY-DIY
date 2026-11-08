# ArmControl API reference

Import:

```python
import doly_arm
```

This page documents the public API exposed by the `doly_arm` Python module.

## Enums

### `ArmErrorType`

Values:

- `Abort`
- `Motor`

### `ArmSide`

Values:

- `Both`
- `Left`
- `Right`

### `ArmState`

Values:

- `Running`
- `Completed`
- `Error`

## Classes

### `ArmData`

`ArmData()` creates a record with writable fields.

**Fields**

- **side: ArmSide**: Arm side.
- **angle: float**: Current angle in degrees.

### `ArmEventListener`

`ArmEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onArmComplete(id: int, side: ArmSide) -> None`
  - Called when an arm command completes successfully.
- `onArmError(id: int, side: ArmSide, error_type: ArmErrorType) -> None`
  - Called when an arm command ends with an error.
- `onArmStateChange(side: ArmSide, state: ArmState) -> None`
  - Called when the arm state changes.
- `onArmMovement(side: ArmSide, degree_change: float) -> None`
  - Called to report incremental motion events.

## Functions

### `add_listener(listener: Optional[ArmEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `ArmEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: ArmEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_complete(cb: Optional[Callable[[int, ArmSide], None]]) -> None`

Set the complete function callback, replacing any previous callable.

- **cb**: Called as `cb(id, side)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_error(cb: Optional[Callable[[int, ArmSide, ArmErrorType], None]]) -> None`

Set the error function callback, replacing any previous callable.

- **cb**: Called as `cb(id, side, error_type)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_state_change(cb: Optional[Callable[[ArmSide, ArmState], None]]) -> None`

Set the state change function callback, replacing any previous callable.

- **cb**: Called as `cb(side, state)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_movement(cb: Optional[Callable[[ArmSide, float], None]]) -> None`

Set the movement function callback, replacing any previous callable.

- **cb**: Called as `cb(side, degree_change)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `init() -> int`

Initialize the arm subsystem.

This must be called once before other control functions.

**Returns**

- `int`:

```text
0: success
1: already initialized
-1: left servo enable pin failed
-2: right servo enable pin failed
-3: ServoMotor initialization failed
-4: channel setup failed
-5: initial release failed
```

**Notes**

- After successful init(), is_active() should return true.


### `dispose() -> int`

Dispose/stop the arm subsystem and release resources. Accepted disposal also
cleans up Arm Python callbacks; rejection retains registrations.

**Returns**

- `int`:

```text
0: success
1: not initialized
-3: callback-context rejection; registrations retained
-5: resource release error after teardown; Python callback cleanup accepted
```


### `is_active() -> bool`

Check whether the subsystem is active (initialized and running).

**Returns**

- `bool`:

```text
true if active, false otherwise.
```


### `abort(side: ArmSide) -> None`

Abort the current operation for a given side.

This is intended as an emergency/stop action.

**Parameters**

- **side**: Arm side to abort (ArmSide.Left/ArmSide.Right/ArmSide.Both).


### `get_max_angle() -> int`

Get maximum allowed angle for the arm.

**Returns**

- `int`:

```text
Maximum angle (degrees).
```


### `set_angle(id: int, side: ArmSide, speed: int, angle: int, with_brake: bool = False) -> int`

Command the arm to move to an angle.

Motion runs asynchronously, but command submission may wait for previous work or
motor readiness. A zero result means the command was accepted, not completed.
Use state and `ArmEventListener.onArmComplete()`, `onArmError()`, and
`onArmStateChange()` to observe the outcome.

**Parameters**

- **id**: User-defined command identifier (echoed back in completion/error callbacks).
- **side**: Arm side to move (ArmSide.Left/ArmSide.Right/ArmSide.Both).
- **speed**: Speed percentage in range [1..100].
- **angle**: Target angle in degrees in range [0..get_max_angle()].
- **with_brake**: If `True`, apply brake/hold behavior at target (implementation-defined). (default: `False`)

**Returns**

- `int`:

```text
0: command accepted
-1: controller inactive or lower-level motor angle failure
-2: speed or lower-level motor parameter/channel failure
-3: angle/side validation or motor availability failure
-4: movement worker could not start
```


Motion error codes can overlap between controller validation and forwarded motor
failures; a code does not uniquely identify the cause.

### `get_state(side: ArmSide) -> ArmState`

Get current operation state for a side.

**Parameters**

- **side**: Arm side.

**Returns**

- `ArmState`:

```text
Current ArmState.
```


### `get_current_angle(side: ArmSide) -> list[ArmData]`

Get the current angle(s) for the requested side(s).

**Parameters**

- **side**: Requested side (ArmSide.Left/ArmSide.Right/ArmSide.Both).

**Returns**

- `list[ArmData]`:

```text
List of ArmData entries. If side == ArmSide.Both, the list may contain 2 elements.
```


### `get_version() -> float`

Get SDK/library version.

Format note from original header: 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

## Lifecycle contract

`init()` and `abort(side)` raise `RuntimeError` from any wrapper callback.
`dispose()` instead returns `-3` there. Call lifecycle operations on the control
thread. Disposal outcomes `0`, `1` and `-5` accept Python callback cleanup; a
negative status must still be reported as an operation failure.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.

