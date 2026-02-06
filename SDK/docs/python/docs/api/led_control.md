# LedControl API reference

Import:

```python
import doly_led
```

This page documents the public API exposed by the `doly_led` Python module.

## Enums

### `LedSide`

Values:

- `Both`
- `Left`
- `Right`

### `LedActivityState`

Values:

- `Free`
- `Running`
- `Completed`

### `LedErrorType`

Values:

- `Abort`

## Classes

### `LedActivity`

**Fields**

- **mainColor**
- **fadeColor**
- **fade_time**
- **state**

### `LedEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onLedComplete(id: int, side: LedSide) -> None`
  - Called when an activity completes.
- `onLedError(id: int, side: LedSide, type: LedErrorType) -> None`
  - Called when an activity fails or is aborted.

## Functions

### `add_listener(listener: LedEventListener, priority: bool = False) -> None`

Register a LedEventListener instance.

**Parameters**

- **listener**: Listener object to register.
- **priority**: If `True`, the listener is inserted with higher priority (called earlier). (default: `False`)


### `remove_listener(listener: LedEventListener) -> None`

Unregister a previously registered LedEventListener instance.

**Parameters**

- **listener**: Listener object to unregister.


### `on_complete(cb: py::function) -> None`

Set a static complete callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_error(cb: py::function) -> None`

Set a static error callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


### `init() -> int`

Initialize the LED subsystem.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized - -1 : R Left GPIO init failed - -2 : G Left GPIO init failed - -3 : B Left GPIO init failed - -4 : R Right GPIO init failed - -5 : G Right GPIO init failed - -6 : B Right GPIO init failed
```


### `dispose() -> int`

Stop the worker thread and release resources.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1..-6 : GPIO pin release failed.
```


### `is_active() -> bool`

Check whether the LED subsystem is active (initialized).

**Returns**

- `bool`:

```text
true if initialized; false otherwise.
```


### `abort(side: LedSide) -> None`

Abort any running activity on the given side.

**Parameters**

- **side**: LED side to abort.


### `process_activity(id: int, side: LedSide, activity: LedActivity) -> None`

Set a LED activity to be processed asynchronously.

This is a non-blocking operation. The internal worker processes the activity and completion/error is reported via LedEvent callbacks.

**Parameters**

- **id**: User-defined activity identifier (forwarded to completion/error events).
- **side**: LED side to run the activity on.
- **activity**: Activity parameters (colors, fade time).


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
