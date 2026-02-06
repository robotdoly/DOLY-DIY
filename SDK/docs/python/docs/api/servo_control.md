# ServoControl API reference

Import:

```python
import doly_servo
```

This page documents the public API exposed by the `doly_servo` Python module.

## Enums

### `ServoId`

Values:

- `Servo0`
- `Servo1`

## Classes

### `ServoEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onServoAbort(id: int, channel: ServoId) -> None`
  - Called when a servo action is aborted.
- `onServoError(id: int, channel: ServoId) -> None`
  - Called when a servo action fails.
- `onServoComplete(id: int, channel: ServoId) -> None`
  - Called when a servo action completes successfully.

## Functions

### `add_listener(listener: ServoEventListener, priority: bool = False) -> None`

Register a class-based listener (kept alive by the binding until removed/cleared).

**Parameters**

- **listener**:
- **priority**:  (default: `False`)


### `remove_listener(listener: ServoEventListener) -> None`

Unregister a class-based listener.

**Parameters**

- **listener**:


### `on_complete(cb: py::function) -> None`

Set a static servo complete callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_abort(cb: py::function) -> None`

Set a static servo abort callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_error(cb: py::function) -> None`

Set a static servo error callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


### `init() -> int`

Initialize the servo subsystem.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : SERVO_0 setup failed - -2 : SERVO_1 setup failed
```


### `set_servo(id: int, channel: ServoId, angle: float, speed: int = 100, invert: bool = False) -> int`

Set a servo target angle.

**Parameters**

- **id**: User-defined action identifier (forwarded to event callbacks).
- **channel**: Servo channel to control.
- **angle**: Target angle (degrees).
- **speed**: Speed percentage (valid range: 0..100). (default: `100`)
- **invert**: If `True`, invert the direction/orientation for the given channel. (default: `False`)

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : max angle exceed error - -2 : speed range error (0..100) - -3 : undefined channel - -4 : not initialized
```


### `abort(channel: ServoId) -> int`

Abort an ongoing servo action.

**Parameters**

- **channel**: Servo channel to abort.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : failed
```


### `release(channel: ServoId) -> int`

Release servo hold (disable holding torque) for a channel.

**Parameters**

- **channel**: Servo channel to release.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : failed - -2 : busy/running
```


### `dispose() -> int`

Dispose/stop the servo subsystem and release resources.

**Returns**

- `int`:

```text
Status code (implementation-defined).
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
