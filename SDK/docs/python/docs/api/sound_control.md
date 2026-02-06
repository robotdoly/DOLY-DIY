# SoundControl API reference

Import:

```python
import doly_sound
```

This page documents the public API exposed by the `doly_sound` Python module.

## Enums

### `SoundState`

Values:

- `Set`
- `Stop`
- `Play`

## Classes

### `SoundEventListener`

Listener interface. Implement this class and register it with `add_listener()` to receive events.

**Methods**

- `onSoundBegin(id: int, volume: float) -> None`
  - Called when playback begins.
- `onSoundComplete(id: int) -> None`
  - Called when playback completes successfully.
- `onSoundAbort(id: int) -> None`
  - Called when playback is aborted.
- `onSoundError(id: int) -> None`
  - Called when a playback error occurs.

## Functions

### `add_listener(listener: SoundEventListener, priority: bool = False) -> None`

Register a SoundEventListener instance.

**Parameters**

- **listener**: Listener object to register.
- **priority**: If `True`, the listener is inserted with higher priority (called earlier). (default: `False`)


### `remove_listener(listener: SoundEventListener) -> None`

Unregister a previously registered SoundEventListener instance.

**Parameters**

- **listener**: Listener object to unregister.


### `on_begin(cb: py::function) -> None`

Set a static begin callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_complete(cb: py::function) -> None`

Set a static complete callback (replaces any previous one).
Tip: If you want multiple handlers, use a Python dispatcher function.

**Parameters**

- **cb**:


### `on_abort(cb: py::function) -> None`

Set a static abort callback (replaces any previous one).
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

Initialize sound control.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already running - -1 : loading failed
```


### `dispose() -> int`

Dispose/stop the sound subsystem and release resources.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not initialized
```


### `play(file_name: str, block_id: int) -> int`

Start playing a sound file (non-blocking).

**Parameters**

- **file_name**: Full path of the sound file.
- **block_id**: User-defined sound identifier forwarded to SoundEvent callbacks.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : sound control not initialized - -2 : file not found
```

**Notes**

- Playback happens asynchronously; use SoundEvent callbacks to track progress.


### `abort() -> None`

Abort the currently playing sound (if any).


### `get_state() -> SoundState`

Get current sound playback state.

**Returns**

- `SoundState`:

```text
Current SoundState.
```


### `is_active() -> bool`

Check whether the sound subsystem is active (initialized).

**Returns**

- `bool`:

```text
true if initialized; false otherwise.
```


### `set_volume(volume: int) -> int`

Set playback volume as a percentage.

**Parameters**

- **volume**: Volume percentage (0..100).

**Returns**

- `int`:

```text
0 on success; negative value on failure.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
