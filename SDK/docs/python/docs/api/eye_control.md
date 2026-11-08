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

`EyeEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onEyeStart(id: int) -> None`
  - Called when an eye action/animation starts.
- `onEyeComplete(id: int) -> None`
  - Called when an eye action/animation completes.
- `onEyeAbort(id: int) -> None`
  - Called when an eye action/animation is aborted/stopped.

## Functions

### `add_listener(listener: Optional[EyeEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `EyeEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: EyeEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_start(cb: Optional[Callable[[int], None]]) -> None`

Set the start function callback, replacing any previous callable.

- **cb**: Called as `cb(id)`; pass `None` to unregister this function callback.
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

### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `init(eye_color: ColorCode, bg_color: ColorCode) -> int`

Initialize the eye subsystem (LCD + eye assets).

This must be called once before other control functions.

**Parameters**

- **eye_color**: Default iris color (see [ColorCode](color.md#colorcode)).
- **bg_color**: Default background color (see [ColorCode](color.md#colorcode)).

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized - -1 : loadEyeFiles failed - -2 : LCD init failed
```


### `abort() -> None`

Abort/terminate the active animation.

This preserves native cancellation behavior. It is not a native worker join or
proof that subsequent image operations are safe. Prefer natural completion before
image changes; avoid concurrent cancellation and image operations.


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
0: success
-1: iris preset lookup failed
-2: background preset lookup failed
```


### `set_iris(shape: IrisShape, color: ColorCode, side: EyeSide) -> int`

Set iris shape and color.

**Parameters**

- **shape**: Built-in iris preset.
- **color**: Shared `doly_color.ColorCode` value.
- **side**: Eye side to render.

**Returns**

- `int`: `0` on success, `-1` if the requested iris style is unavailable,
  or `-2` if the requested shape is unavailable.


### `get_iris_position(side: EyeSide) -> tuple[int, int, int]`

Get the current iris center position for a given side. Requires idle Eye processing.

- **side**: `EyeSide.Left` or `EyeSide.Right`.

**Returns**

`(status, x, y)`: status `0` on success, or `-1` for a wrong side (including Both).
Check status before using the coordinates. The top-left corner of each display
is x=0, y=0; the display center is (120, 120).

### `set_background(color: Color) -> int`

Set background color.

**Parameters**

- **color**: A shared [doly_color.Color](color.md#color) value.

**Returns**

- `int`: `0` on normal return. The ownership restrictions and exceptions below
  still apply.


### `set_iris_image(eye_image: VContent, side: EyeSide) -> int`

Set iris image content for a given side.

**Parameters**

- **eye_image**: Iris image content.
- **side**: Target eye side.

**Returns and exceptions**

Returns the nonnegative native status on success (normally `0`). A `None` image
or invalid image dimensions/channels/frames raises `ValueError`. A negative native
assignment result becomes `RuntimeError`, with old/new owners retained and further
guarded Eye operations blocked. Initialization and idle processing are required.
See [image ownership and restrictions](#image-ownership-and-restrictions).

### `set_lid_image(lid_image: VContent, is_top: bool, side: EyeSide) -> int`

Set eyelid image content for a given side.

**Parameters**

- **lid_image**: Lid image content.
- **is_top**: True for top lid, `False` for bottom lid.
- **side**: Target eye side.

**Returns and exceptions**

Returns the nonnegative native status on success (normally `0`). A `None` image
or invalid image dimensions/channels/frames raises `ValueError`. A negative native
assignment result becomes `RuntimeError`, with old/new owners retained and further
guarded Eye operations blocked. Initialization and idle processing are required.
See [image ownership and restrictions](#image-ownership-and-restrictions).

### `set_background_image(bg_image: VContent, side: EyeSide) -> int`

Set background image content for a given side.

**Parameters**

- **bg_image**: Background image content.
- **side**: Target eye side.

**Returns and exceptions**

Returns the nonnegative native status on success (normally `0`). A `None` image
or invalid image dimensions/channels/frames raises `ValueError`. A negative native
assignment result becomes `RuntimeError`, with old/new owners retained and further
guarded Eye operations blocked. Initialization and idle processing are required.
See [image ownership and restrictions](#image-ownership-and-restrictions).

### `set_animation(id: int, name: str) -> int`

Submit a named eye animation. Playback runs asynchronously, but submission can
prepare image assets and wait for previous processing or rendering. A successful
submission does not mean completion. Start/complete/abort are reported via
EyeEvent/EyeEventListener.

**Parameters**

- **id**: User-defined id forwarded to event callbacks.
- **name**: Animation name; use a string constant from
  [doly_eye.expressions](#expression-constants).

**Returns**

- `int`:

```text
0: animation accepted
-1: animation not found or required event-image preparation failed
```


### `set_position(side: EyeSide, centerX: int, centerY: int, scaleX: float = 1.0, scaleY: float = 1.0, lid_top_end: int = 0, lid_bot_start: int = 240) -> int`

Set eye position and scale.
Initialize Eye first and wait for idle processing.

- **side**: `EyeSide.Left`, `EyeSide.Right` or `EyeSide.Both`.
- **centerX**, **centerY**: Signed integer center coordinates; native accepted range
  is -250..250. The top-left is (0, 0), display center (120, 120).
- **scaleX**, **scaleY**: Finite, positive scale factors, default `1.0`.
- **lid_top_end**: Unsigned 8-bit top-lid Y end, default `0`.
- **lid_bot_start**: Unsigned 8-bit bottom-lid Y start, default `240`.

**Returns and exceptions**

Native integer status: `0` on success, `-1` for out-of-range center coordinates.
Nonfinite, nonpositive or unsafe scale
magnitudes raise `ValueError` before rendering. Inactive Eye, busy guarded operations
or uncertain image ownership raise `RuntimeError`.

### `dispose() -> None`

Clean up Eye Python callbacks/listeners only. This does not call native disposal,
Abort, image detachment, rendering stop or a native worker join. Image owners
remain retained. From a wrapper callback this performs nonblocking logical cleanup;
outside callbacks it waits only for admitted Python callback entries.

### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

## Image ownership and restrictions

Use [VContent](vcontent.md) for raw image assignment. Initialize Eye before image
assignment/position rendering and use idle processing for raw assignments,
iris-position reads/rendering, and attached-image frame replacement/active-frame
index reads. Python callers share a mutex; callback callers try-lock and raise
`RuntimeError` when busy rather than blocking. Preset and animation operations
retain native replacement/cancellation behavior, not a worker-join guarantee.

All assigned images require nonempty frames, a valid active-frame index, converted
12-bit color metadata, and exact frame lengths. RGB channels must each be 0..15;
alpha, where present, remains 0..255. Each channel occupies a separate byte:

| Slot | Dimensions | Channels |
|---|---|---|
| Background | Exactly 240×240 | RGB |
| Top/bottom lid | Width 240, height 1–240 | RGBA |
| Iris | At least 2×2 | RGBA |

For each frame the byte count must equal width × height × channel count.
Validation checks metadata, dimensions, channel layout, active-frame index, and
frame lengths. It does not inspect individual RGB channel values; maintaining
the required values is the caller's responsibility. `None` or invalid structural
properties raise `ValueError` before native indexing.
Eye retains native shared ownership for each slot/side; dropping Python
references does not free attached content. Replacement retains old owners through
the native call. If assignment fails with uncertain native ownership, old and new
owners remain retained and subsequent guarded Eye operations raise `RuntimeError`.
Stop image operations after such a failure.

Final image owners remain resident because there is no supported detach-and-join
API. Callback disposal never releases them. Preset changes may conservatively
retain previous owners, which remain protected from mutation while processing.
Do not mutate attached images during active processing or through external C++
callers. Do not operate LCD and Eye concurrently.

Natural animation completion is preferred before image changes. A false
`is_animating()` result, Abort, and wrapper mutexes are not a native join or complete
shutdown proof. Native render-busy checks, cancellation and shutdown races remain
limitations; wrapper coordination covers Python callers, not native workers or
external C++ users.

## Lifecycle contract

`init()`, `abort()` and `set_animation()` raise `RuntimeError` from any wrapper
callback. Schedule these on the application control thread. `dispose()` is
callback-only as described above; it is not renderer shutdown. The usual callback
replacement, resident proxy and unraisable-exception limitations apply.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.

## Expression constants

`doly_eye.expressions` exposes the following 142 string constants. Pass the
constant to `set_animation(id, name)`; values with spaces must not be inferred
from the identifier spelling. These are names, not callables or proof that the
required animation asset is installed.

| Constant | String value |
|---|---|
| `expressions.ADMIRING` | `"ADMIRING"` |
| `expressions.AGGRAVATED` | `"AGGRAVATED"` |
| `expressions.ANNOYED` | `"ANNOYED"` |
| `expressions.ANXIOUS` | `"ANXIOUS"` |
| `expressions.ATTENTION` | `"ATTENTION"` |
| `expressions.ATTENTION_LEFT` | `"ATTENTION LEFT"` |
| `expressions.ATTENTION_RIGHT` | `"ATTENTION RIGHT"` |
| `expressions.AWAKE_L` | `"AWAKE L"` |
| `expressions.AWAKE_R` | `"AWAKE R"` |
| `expressions.BATTERY_LOW` | `"BATTERY LOW"` |
| `expressions.BLINK` | `"BLINK"` |
| `expressions.BLINK_BIG` | `"BLINK BIG"` |
| `expressions.BLINK_L` | `"BLINK L"` |
| `expressions.BLINK_ONLY` | `"BLINK ONLY"` |
| `expressions.BLINK_R` | `"BLINK R"` |
| `expressions.BLINK_SLOW` | `"BLINK SLOW"` |
| `expressions.BLUE` | `"BLUE"` |
| `expressions.BUGGED` | `"BUGGED"` |
| `expressions.BUMP` | `"BUMP"` |
| `expressions.BURNED_UP` | `"BURNED UP"` |
| `expressions.CAUTIOUS` | `"CAUTIOUS"` |
| `expressions.CAUTIOUS_DOWN` | `"CAUTIOUS DOWN"` |
| `expressions.CAUTIOUS_DOWN_LEFT` | `"CAUTIOUS DOWN LEFT"` |
| `expressions.CAUTIOUS_DOWN_RIGHT` | `"CAUTIOUS DOWN RIGHT"` |
| `expressions.CAUTIOUS_LEFT` | `"CAUTIOUS LEFT"` |
| `expressions.CAUTIOUS_RIGHT` | `"CAUTIOUS RIGHT"` |
| `expressions.CAUTIOUS_UP` | `"CAUTIOUS UP"` |
| `expressions.CHAOTIC` | `"CHAOTIC"` |
| `expressions.CHEERFUL` | `"CHEERFUL"` |
| `expressions.COLOR_CHANGE` | `"COLOR CHANGE"` |
| `expressions.CONCENTRATE` | `"CONCENTRATE"` |
| `expressions.CONFUSED` | `"CONFUSED"` |
| `expressions.CRAZY_ABOUT` | `"CRAZY ABOUT"` |
| `expressions.CRUSHED` | `"CRUSHED"` |
| `expressions.CYCLOPS_L` | `"CYCLOPS L"` |
| `expressions.CYCLOPS_R` | `"CYCLOPS R"` |
| `expressions.DAMAGED` | `"DAMAGED"` |
| `expressions.DEJECTED` | `"DEJECTED"` |
| `expressions.DELIGHTED` | `"DELIGHTED"` |
| `expressions.DEMORALIZED` | `"DEMORALIZED"` |
| `expressions.DEPRESSED` | `"DEPRESSED"` |
| `expressions.DESTROYED` | `"DESTROYED"` |
| `expressions.DISCOVER` | `"DISCOVER"` |
| `expressions.DISAPPOINTED` | `"DISAPPOINTED"` |
| `expressions.DIZZY_L` | `"DIZZY L"` |
| `expressions.DIZZY_R` | `"DIZZY R"` |
| `expressions.DOWN` | `"DOWN"` |
| `expressions.DROP_CENTER` | `"DROP CENTER"` |
| `expressions.DROWSY` | `"DROWSY"` |
| `expressions.ELEVATED_I` | `"ELEVATED I"` |
| `expressions.ELEVATED_O` | `"ELEVATED O"` |
| `expressions.EMPTY` | `"EMPTY"` |
| `expressions.EXCITED` | `"EXCITED"` |
| `expressions.FED_UP` | `"FED UP"` |
| `expressions.FINE` | `"FINE"` |
| `expressions.FIREMAN` | `"FIREMAN"` |
| `expressions.FLAME` | `"FLAME"` |
| `expressions.FOCUS` | `"FOCUS"` |
| `expressions.FRIGHTENED` | `"FRIGHTENED"` |
| `expressions.FRUSTRATED` | `"FRUSTRATED"` |
| `expressions.FUMING_L` | `"FUMING L"` |
| `expressions.FUMING_R` | `"FUMING R"` |
| `expressions.FURIOUS` | `"FURIOUS"` |
| `expressions.GET_BIGGER` | `"GET BIGGER"` |
| `expressions.GET_SMALLER` | `"GET SMALLER"` |
| `expressions.HAPPY` | `"HAPPY"` |
| `expressions.HEARTS` | `"HEARTS"` |
| `expressions.HEATED` | `"HEATED"` |
| `expressions.HOPELESS` | `"HOPELESS"` |
| `expressions.HOSTILE` | `"HOSTILE"` |
| `expressions.IMPACT_F` | `"IMPACT F"` |
| `expressions.IMPACT_L` | `"IMPACT L"` |
| `expressions.IMPACT_R` | `"IMPACT R"` |
| `expressions.IMPATIENT` | `"IMPATIENT"` |
| `expressions.INJURED` | `"INJURED"` |
| `expressions.IRRITATED` | `"IRRITATED"` |
| `expressions.JEALOUS_L` | `"JEALOUS L"` |
| `expressions.JEALOUS_R` | `"JEALOUS R"` |
| `expressions.LEFT` | `"LEFT"` |
| `expressions.LIDS_DOWN_5S` | `"LIDS DOWN 5S"` |
| `expressions.LOOK_AHEAD` | `"LOOK AHEAD"` |
| `expressions.LOOK_AHEAD_SLOW` | `"LOOK AHEAD SLOW"` |
| `expressions.LOOK_DOWN` | `"LOOK DOWN"` |
| `expressions.LOOK_DOWN_L` | `"LOOK DOWN L"` |
| `expressions.LOOK_DOWN_R` | `"LOOK DOWN R"` |
| `expressions.LOOK_LEFT` | `"LOOK LEFT"` |
| `expressions.LOOK_RIGHT` | `"LOOK RIGHT"` |
| `expressions.LOOK_UP` | `"LOOK UP"` |
| `expressions.LOOK_UP_L` | `"LOOK UP L"` |
| `expressions.LOOK_UP_R` | `"LOOK UP R"` |
| `expressions.LOW` | `"LOW"` |
| `expressions.LOW_LONG` | `"LOW LONG"` |
| `expressions.MELANCHOLY` | `"MELANCHOLY"` |
| `expressions.MID_DOWN` | `"MID DOWN"` |
| `expressions.MID_UP` | `"MID UP"` |
| `expressions.MID_UP_L` | `"MID UP L"` |
| `expressions.MID_UP_R` | `"MID UP R"` |
| `expressions.MIXED_UP` | `"MIXED UP"` |
| `expressions.MOODY_LR` | `"MOODY LR"` |
| `expressions.MOODY_RL` | `"MOODY RL"` |
| `expressions.NERVOUS` | `"NERVOUS"` |
| `expressions.OFFENDED` | `"OFFENDED"` |
| `expressions.OUTRAGED` | `"OUTRAGED"` |
| `expressions.OVERJOYED` | `"OVERJOYED"` |
| `expressions.PANICKY` | `"PANICKY"` |
| `expressions.PASSIONATE` | `"PASSIONATE"` |
| `expressions.PHOTO` | `"PHOTO"` |
| `expressions.POLICE` | `"POLICE"` |
| `expressions.PUZZLED` | `"PUZZLED"` |
| `expressions.PUMPKIN` | `"PUMPKIN"` |
| `expressions.RIGHT` | `"RIGHT"` |
| `expressions.ROLL_UD` | `"ROLL UD"` |
| `expressions.SCAN` | `"SCAN"` |
| `expressions.SHAKE_FRONT` | `"SHAKE FRONT"` |
| `expressions.SHAKE_LR` | `"SHAKE LR"` |
| `expressions.SHAKE_UD` | `"SHAKE UD"` |
| `expressions.SHOCKED` | `"SHOCKED"` |
| `expressions.SHY` | `"SHY"` |
| `expressions.SLEEP` | `"SLEEP"` |
| `expressions.SLEEPY` | `"SLEEPY"` |
| `expressions.SNEEZE` | `"SNEEZE"` |
| `expressions.SPARKLING` | `"SPARKLING"` |
| `expressions.SQUINT_RL` | `"SQUINT RL"` |
| `expressions.STORMING` | `"STORMING"` |
| `expressions.SUNGLASS` | `"SUNGLASS"` |
| `expressions.SUNGLASS_LR` | `"SUNGLASS LR"` |
| `expressions.SUNGLASS_UPR` | `"SUNGLASS UPR"` |
| `expressions.THINK` | `"THINK"` |
| `expressions.THREATENED_L` | `"THREATENED L"` |
| `expressions.THREATENED_R` | `"THREATENED R"` |
| `expressions.THRILLED` | `"THRILLED"` |
| `expressions.THROWN` | `"THROWN"` |
| `expressions.TIRED` | `"TIRED"` |
| `expressions.TROUBLED` | `"TROUBLED"` |
| `expressions.TURNED_ON` | `"TURNED ON"` |
| `expressions.UNCOMFORTABLE` | `"UNCOMFORTABLE"` |
| `expressions.UNHAPPY` | `"UNHAPPY"` |
| `expressions.UPSET` | `"UPSET"` |
| `expressions.VR` | `"VR"` |
| `expressions.WAKE_WORD` | `"WAKE WORD"` |
| `expressions.WORKOUT` | `"WORKOUT"` |
| `expressions.ZOOM_IN` | `"ZOOM IN"` |


