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

`LedActivity()` creates an activity with writable fields. Colors use the shared
[doly_color.Color](color.md#color) type. Set the two colors equal to avoid fading;
in that case `fade_time` acts as a waiting time.

**Fields**

- **mainColor: doly_color.Color**: Start color, initially black.
- **fadeColor: doly_color.Color**: End color, initially black.
- **fade_time: int**: Unsigned 16-bit milliseconds, initially 0.
- **state: LedActivityState**: Initially `LedActivityState.Free`.

### `LedEventListener`

`LedEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onLedComplete(id: int, side: LedSide) -> None`
  - Called when an activity completes.
- `onLedError(id: int, side: LedSide, type: LedErrorType) -> None`
  - Called when an activity fails or is aborted.

## Functions

### `add_listener(listener: Optional[LedEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `LedEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: LedEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_complete(cb: Optional[Callable[[int, LedSide], None]]) -> None`

Set the complete function callback, replacing any previous callable.

- **cb**: Called as `cb(id, side)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_error(cb: Optional[Callable[[int, LedSide, LedErrorType], None]]) -> None`

Set the error function callback, replacing any previous callable.

- **cb**: Called as `cb(id, side, type)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

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
0: normal disposal result; GPIO release failures are not reported
1: not initialized
-7: callback-context rejection; registrations retained
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

Process a LED activity. A zero `fade_time` applies the color synchronously;
completion can be delivered before this call returns. A nonzero fade uses worker
threads. Submission may acquire locks or wait during worker-start failure handling,
so it has no unconditional non-blocking guarantee. Completion/error is reported
via LedEvent callbacks.

**Parameters**

- **id**: User-defined activity identifier (forwarded to completion/error events).
- **side**: LED side to run the activity on.
- **activity**: Activity parameters (colors, fade time), copied by value for
  native processing. Changing the original object does not update that copy.


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

## Lifecycle contract

`init()` and `abort(side)` raise `RuntimeError` from any wrapper callback.
`dispose()` returns `-7` there without changing registrations. Results `0/1`
accept Python callback cleanup. The wrapper also accepts resource codes `-1`
through `-6` for compatibility, but the current controller does not report GPIO
release failures through those codes. A zero result does not prove successful
GPIO release. Only completion/error listener methods are supported here; native
animation callbacks are not exposed as Python APIs.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.


