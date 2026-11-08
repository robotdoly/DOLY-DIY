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

`ServoEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onServoAbort(id: int, channel: ServoId) -> None`
  - Called when a servo action is aborted.
- `onServoError(id: int, channel: ServoId) -> None`
  - Called when a servo action fails.
- `onServoComplete(id: int, channel: ServoId) -> None`
  - Called when a servo action completes successfully.

## Functions

### `add_listener(listener: Optional[ServoEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `ServoEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: ServoEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_complete(cb: Optional[Callable[[int, ServoId], None]]) -> None`

Set the complete function callback, replacing any previous callable.

- **cb**: Called as `cb(id, channel)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_abort(cb: Optional[Callable[[int, ServoId], None]]) -> None`

Set the abort function callback, replacing any previous callable.

- **cb**: Called as `cb(id, channel)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_error(cb: Optional[Callable[[int, ServoId], None]]) -> None`

Set the error function callback, replacing any previous callable.

- **cb**: Called as `cb(id, channel)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `init() -> int`

Initialize the servo subsystem.

**Returns**

- `int`:

```text
0: success
1: already initialized
-1: ServoMotor initialization failed
-2: Servo0 setup failed
-3: Servo1 setup failed
```


### `set_servo(id: int, channel: ServoId, angle: float, speed: int = 100, invert: bool = False) -> int`

Submit a servo target angle. Motion runs asynchronously, but submission may wait
for previous work on the channel. A zero result means accepted, not completed.

**Parameters**

- **id**: User-defined action identifier (forwarded to event callbacks).
- **channel**: Servo channel to control.
- **angle**: Target angle (degrees).
- **speed**: Speed percentage (valid range: 1..100). (default: `100`)
- **invert**: If `True`, invert the direction/orientation for the given channel. (default: `False`)

**Returns**

- `int`:

```text
0: command accepted
-1: angle exceeds the configured motor maximum
-2: speed or lower-level motor parameter/channel failure
-3: undefined channel or unavailable motor
-4: controller inactive, preceding work could not finish, or movement worker could not start
```


The controller can forward lower-level motor errors, so these codes do not uniquely
identify a failure's cause.

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
0: hold released
-1: undefined channel or servo
-2: channel still active or PWM release failed
```


### `dispose() -> int`

Dispose/stop the servo subsystem and release resources.

**Returns**

- `int`:

```text
0: success
1: not initialized
-4: callback-context rejection; registrations retained
-5: resource release error after teardown; Python callback cleanup accepted
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

### `servo_complete(id: int, channel: ServoId) -> None`

Dispatch the corresponding servo completion event with the action **id** and servo
**channel**. This is event dispatch, not a motor command or proof of physical
completion.

### `servo_abort(id: int, channel: ServoId) -> None`

Dispatch the corresponding servo abort event with the action **id** and servo
**channel**. This is event dispatch, not a motor command or proof of physical
completion.

`servo_abort` dispatches an event; `abort(channel)` cancels a motor action.

### `servo_error(id: int, channel: ServoId) -> None`

Dispatch the corresponding servo error event with the action **id** and servo
**channel**. This is event dispatch, not a motor command or proof of physical
completion.

## Lifecycle contract

`init()`, `abort(channel)` and `release(channel)` raise `RuntimeError` from any
wrapper callback. `dispose()` returns `-4` there and retains registrations.
Disposal outcomes `0`, `1` and `-5` accept component callback cleanup.
A timed dwell is not proof that a servo finished. `release()` can report `-2`
for either a running channel or a PWM release failure.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.


