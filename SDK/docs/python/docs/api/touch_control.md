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

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onTouchEvent(side: TouchSide, state: TouchState) -> None`
  - Called on touch state changes.
- `onTouchActivityEvent(side: TouchSide, activity: TouchActivity) -> None`
  - Called on detected touch activity (high-level pattern).

## Functions

### `add_listener(listener: TouchEventListener, priority: bool = False) -> None`

Register a TouchEventListener instance.

**Parameters**

- **listener**: Listener object to register.
- **priority**: If `True`, the listener is inserted with higher priority (called earlier). (default: `False`)


### `remove_listener(listener: TouchEventListener) -> None`

Unregister a previously registered TouchEventListener instance.

**Parameters**

- **listener**: Listener object to unregister.


### `on_touch(cb: py::function) -> None`

Set a static touch callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_touch_activity(cb: py::function) -> None`

Set a static touch-activity callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


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
Status code: - 0 : success - -1 : left sensor GPIO release failed - -2 : right sensor GPIO release failed
```


### `is_active() -> bool`

Check whether the touch controller is active (initialized and running).

**Returns**

- `bool`:

```text
true if active; false otherwise.
```


### `is_touched(side: TouchSide) -> bool`

Check whether the given side is currently touched.

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
