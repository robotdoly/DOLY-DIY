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

**Fields**

- **side**
- **angle**

### `ArmEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

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

### `add_listener(listener: ArmEventListener, priority: bool = False) -> None`

Register a listener object to receive arm events.

**Parameters**

- **listener**: Pointer to a listener instance (must not be null).
- **priority**: If `True`, the listener is inserted with priority ordering (implementation-defined). (default: `False`)

**Notes**

- Callbacks are typically invoked from an internal worker/event thread.


### `remove_listener(listener: ArmEventListener) -> None`

Unregister a listener object.

**Parameters**

- **listener**: Pointer previously passed to AddListener().


### `on_complete(cb: py::function) -> None`

Set a static complete callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_error(cb: py::function) -> None`

Set a static error callback (replaces any previous one). Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_state_change(cb: py::function) -> None`

Set a static state change callback (replaces any previous one). Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_movement(cb: py::function) -> None`

Set a static movement callback (replaces any previous one). Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


### `init() -> int`

Initialize the arm subsystem.

This must be called once before other control functions.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized - -1 : left servo enable pin set failed - -2 : right servo enable pin set failed
```

**Notes**

- After successful init(), isActive() should return true.


### `dispose() -> int`

Dispose/stop the arm subsystem and release resources. Removes the call back functions.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not initialized
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

- **side**: Arm side to abort (LEFT/RIGHT/BOTH).


### `get_max_angle() -> int`

Get maximum allowed angle for the arm.

**Returns**

- `int`:

```text
Maximum angle (degrees).
```


### `set_angle(id: int, side: ArmSide, speed: int, angle: int, with_brake: bool = False) -> int`

Command the arm to move to an angle.

The operation is handled asynchronously (non-blocking). Results are reported via: - ArmEventListener::onArmComplete() - ArmEventListener::onArmError() - ArmEventListener::onArmStateChange()

**Parameters**

- **id**: User-defined command identifier (echoed back in completion/error callbacks).
- **side**: Arm side to move (LEFT/RIGHT/BOTH).
- **speed**: Speed percentage in range [1..100].
- **angle**: Target angle in degrees in range [0..getMaxAngle()].
- **with_brake**: If `True`, apply brake/hold behavior at target (implementation-defined). (default: `False`)

**Returns**

- `int`:

```text
Status code: - 0 : success (command accepted) - -1 : not active (init() not called or subsystem not running) - -2 : speed out of range - -3 : angle out of range
```


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

- **side**: Requested side (LEFT/RIGHT/BOTH).

**Returns**

- `list[ArmData]`:

```text
Vector of ArmData entries. If side == BOTH, the vector may contain 2 elements.
```


### `get_version() -> float`

Get SDK/library version.

Format note from original header: 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
