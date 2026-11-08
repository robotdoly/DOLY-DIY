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

`SoundEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onSoundBegin(id: int, volume: float) -> None`
  - Called when processing of a playback request begins, before decoder/device
    startup succeeds. This does not confirm that audio has started. `volume`
    is the stored volume multiplier; `set_volume(80)` yields `0.8`.
- `onSoundComplete(id: int) -> None`
  - Called when playback completes successfully.
- `onSoundAbort(id: int) -> None`
  - Called when playback is aborted.
- `onSoundError(id: int) -> None`
  - Called when a playback error occurs.

## Functions

### `add_listener(listener: Optional[SoundEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `SoundEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: SoundEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_begin(cb: Optional[Callable[[int, float], None]]) -> None`

Set the begin function callback, replacing any previous callable.

- **cb**: Called as `cb(id, volume)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_complete(cb: Optional[Callable[[int], None]]) -> None`

Set the complete function callback, replacing any previous callable.

- **cb**: Called as `cb(id)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_abort(cb: Optional[Callable[[int], None]]) -> None`

Set the abort function callback, replacing any previous callable.

- **cb**: Called as `cb(id)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_error(cb: Optional[Callable[[int], None]]) -> None`

Set the error function callback, replacing any previous callable.

- **cb**: Called as `cb(id)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `init() -> int`

Initialize sound control.

**Returns**

- `int`:

```text
0: success
1: already running
-1: loading failed
-2: mixer setup failed
-3: subsystem stopping
```


### `dispose() -> int`

Dispose/stop the sound subsystem and release resources.

**Returns**

- `int`:

```text
0: success
1: not initialized
-1: callback-context rejection; registrations retained
-2: post-drain decoder error; Python callback cleanup accepted
-3: ambiguous stopping/lifecycle failure; registrations retained
```


### `play(file_name: str, block_id: int) -> int`

Submit a file for asynchronous playback. Initialize sound control first.
Replacing playback may wait for device operations and synchronous abort handlers.
The native decoder output and playback device use signed 16-bit PCM, stereo,
at 48 kHz. This output configuration does not require input files to use a
48-kHz sample rate and does not establish support for every input format or
sample rate. A successful submission is not proof of completed playback;
observe state and callbacks.

**Parameters**

- **file_name**: Absolute or relative sound-file path. Relative paths resolve from the process working directory.
- **block_id**: User-defined sound identifier forwarded to SoundEvent callbacks.

**Returns**

- `int`:

```text
0: accepted
-1: sound control not initialized
-2: file not found
-3: active/deferred ID reuse, playback replacement or scheduling failure
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

Store the playback volume as a percentage. It is applied when a playback request
starts its device; this setter does not immediately adjust already-running playback.

**Parameters**

- **volume**: Integer percentage; use 0..100. Python converts it to an unsigned
  8-bit integer (0..255). Native values 101..255 clamp to 100; out-of-range or
  incompatible arguments fail Python conversion.

**Returns**

- `int`:

```text
0 on normal return.
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

`init()` and `abort()` raise `RuntimeError` from any wrapper callback.
`dispose()` returns `-1` there. Native `0/1/-2` accept callback cleanup;
`-1/-3` retain surviving registrations. Because `-3` can describe different
lifecycle stages, do not treat it as proof of completed cleanup. Retained Python
targets can remain until explicit removal or a later accepted disposal.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.


