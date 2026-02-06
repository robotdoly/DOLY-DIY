# EyeControl API reference

Import:

```python
import doly_eye
```

This page documents the public API exposed by the `doly_eye` Python module.

## Enums

### `EyeSide`

Values:

- `Both`
- `Left`
- `Right`

### `IrisShape`

Values:

- `Classic`
- `Modern`
- `Space`
- `Orbit`
- `Glow`
- `Digi`

## Classes

### `EyeEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onEyeStart(id: int) -> None`
  - Called when an eye action/animation starts.
- `onEyeComplete(id: int) -> None`
  - Called when an eye action/animation completes.
- `onEyeAbort(id: int) -> None`
  - Called when an eye action/animation is aborted/stopped.

## Functions

### `add_listener(listener: EyeEventListener, priority: bool = False) -> None`

Register an EyeEventListener to receive events.

**Parameters**

- **listener**: Listener instance.
- **priority**: If `True`, listener is placed at the front of the dispatch list. (default: `False`)


### `remove_listener(listener: EyeEventListener) -> None`

Unregister a previously added EyeEventListener.

**Parameters**

- **listener**: Listener instance to remove.


### `on_start(cb: py::function) -> None`

Set a static eye-start callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_complete(cb: py::function) -> None`

Set a static eye-complete callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_abort(cb: py::function) -> None`

Set a static eye-abort callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `clear_listeners() -> None`

Unregister all static callbacks and clear stored Python functions.


### `init(eye_color: ColorCode, bg_color: ColorCode) -> int`

Initialize the eye subsystem (LCD + eye assets).

This must be called once before other control functions.

**Parameters**

- **eye_color**: Default iris color (see Color.h).
- **bg_color**: Default background color (see Color.h).

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized - -1 : loadEyeFiles failed - -2 : LCD init failed
```


### `abort() -> None`

Abort/terminate the active animation.

This is intended as a stop/emergency action for ongoing animations.


### `is_active() -> bool`

Check whether the subsystem is active (initialized and running).

**Returns**

- `bool`:

```text
true if initialized, false otherwise.
```


### `is_animating() -> bool`

Check whether an animation is currently running.

**Returns**

- `bool`:

```text
true if an animation continues, false otherwise.
```


### `set_eyes(shape: IrisShape, iris_color: ColorCode, bg_color: ColorCode) -> int`

Set both iris and background to built-in presets.

**Parameters**

- **shape**: Built-in iris shape preset.
- **iris_color**: Iris color.
- **bg_color**: Background color.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : getIris() failed, out of array - -2 : getBackground() failed, out of array
```


### `set_iris(shape: IrisShape, color: ColorCode, side: EyeSide) -> int`

Set iris shape and color.

**Parameters**

- **shape**:
- **color**:
- **side**:

**Returns**

- `int`


### `get_iris_position(side: EyeSide) -> int`

Get the current iris center position for a given side.

**Parameters**

- **side**: Target eye side (LEFT or RIGHT).

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : wrong side (accepts Left or Right)
```

**Notes**

- The top-left corner of each display is defined as x=0, y=0.


### `set_background(color: Color) -> int`

Set background color.

**Parameters**

- **color**:

**Returns**

- `int`


### `set_iris_image(eye_image: VContent, side: EyeSide) -> int`

Set iris image content for a given side.

**Parameters**

- **eye_image**: Iris image content.
- **side**: Target eye side.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : null image pointer
```


### `set_lid_image(lid_image: VContent, is_top: bool, side: EyeSide) -> int`

Set eyelid image content for a given side.

**Parameters**

- **lid_image**: Lid image content.
- **is_top**: True for top lid, `False` for bottom lid.
- **side**: Target eye side.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : null image pointer
```


### `set_background_image(bg_image: VContent, side: EyeSide) -> int`

Set background image content for a given side.

**Parameters**

- **bg_image**: Background image content.
- **side**: Target eye side.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : null image pointer
```


### `set_animation(id: int, name: str) -> int`

Start a named eye animation (non-blocking).

The animation runs on an internal worker thread. Start/complete/abort are reported via EyeEvent/EyeEventListener.

**Parameters**

- **id**: User-defined id forwarded to event callbacks.
- **name**: Animation name (see EyeExpressions).

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : can not found animation, check name
```


### `set_position(side: Any, centerX: Any, centerY: Any, scaleX: Any = 1.0f, scaleY: Any = 1.0f, lid_top_end: int = 0, lid_bot_start: int = 240) -> None`

Set eye position and scale.

**Parameters**

- **side**:
- **centerX**:
- **centerY**:
- **scaleX**:  (default: `1.0f`)
- **scaleY**:  (default: `1.0f`)
- **lid_top_end**:  (default: `0`)
- **lid_bot_start**:  (default: `240`)


### `dispose() -> None`

Dispose wrapper callbacks/listeners (EyeControl has no dispose function).


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
