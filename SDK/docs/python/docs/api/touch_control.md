# TouchControl API reference

Import:

```python
import doly_touch
```

This page documents the public API exposed by the `doly_touch` Python module.

## Enums

### `TouchSide`

Values:

- `Both`
- `Left`
- `Right`

### `TouchState`

Values:

- `Up`
- `Down`

### `TouchActivity`

Values:

- `Patting`
- `Disturb`

## Classes

### `TouchEventListener`

`TouchEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onTouchEvent(side: TouchSide, state: TouchState) -> None`
  - Called on touch state changes.
- `onTouchActivityEvent(side: TouchSide, activity: TouchActivity) -> None`
  - Called on detected touch activity (high-level pattern).

## Functions

### `add_listener(listener: Optional[TouchEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `TouchEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: TouchEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_touch(cb: Optional[Callable[[TouchSide, TouchState], None]]) -> None`

Set the touch function callback, replacing any previous callable.

- **cb**: Called as `cb(side, state)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_touch_activity(cb: Optional[Callable[[TouchSide, TouchActivity], None]]) -> None`

Set the touch activity function callback, replacing any previous callable.

- **cb**: Called as `cb(side, activity)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `init() -> int`

Initialize the touch controller and start the worker thread.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized (no-op) - -1 : left sensor GPIO initialization failed - -2 : right sensor GPIO initialization failed
```


### `dispose() -> int`

Stop the worker thread and release resources.

**Returns**

- `int`:

```text
0: normal disposal result; GPIO release failures are not reported
-3: callback-context rejection; registrations retained
```


### `is_active() -> bool`

Check whether the touch controller is active (initialized and running).

**Returns**

- `bool`:

```text
true if active; false otherwise.
```


### `is_touched(side: TouchSide) -> bool`

Check whether the given side is currently touched. With `TouchSide.Both`, the
result is true only when both sides are touched, not when either side is touched.

**Parameters**

- **side**: Sensor side to query.

**Returns**

- `bool`:

```text
true if touched; false otherwise.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

## Lifecycle contract

`init()` raises `RuntimeError` from any wrapper callback. `dispose()` returns
`-3` there and retains registrations. A normal result of `0` permits Python
callback cleanup. The wrapper also accepts `1/-1/-2` for compatibility, but
these are not emitted by the current native disposal path. GPIO release failures
are not reported by that path, so `0` does not prove successful resource release.
Perform lifecycle work on the application control thread.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.

