# EdgeControl API reference

Import:

```python
import doly_edge
```

This page documents the public API exposed by the `doly_edge` Python module.

## Enums

### `GpioState`

Alias of [doly_io.GpioState](io_control.md#gpiostate), not a separate enum type.

Values:

- `Low`
- `High`

### `GapDirection`

Values:

- `Front`
- `Front_Left`
- `Front_Right`
- `Back`
- `Back_Left`
- `Back_Right`
- `Left`
- `Right`
- `Cross_Left`
- `Cross_Right`
- `All`

### `SensorId`

Values:

- `Back_Left`
- `Back_Right`
- `Front_Left`
- `Front_Right`

## Classes

### `IrSensor`

`IrSensor()` creates a record with writable fields. `__repr__() -> str` returns
`<IrSensor id=… state=…>` using numeric enum values.

**Fields**

- **id: SensorId**: Sensor location.
- **state: GpioState**: Current ground-detection state.

### `EdgeEventListener`

`EdgeEventListener()` constructs the listener base. Subclass it and register it with `add_listener()` to receive events.

**Methods**

- `onEdgeChange(sensors: list[IrSensor]) -> None`
  - Called when the set of IR sensor states changes.
- `onGapDetect(gap_type: GapDirection) -> None`
  - Called when a gap (e.g. drop-off, missing surface) is detected and categorized.

## Functions

### `add_listener(listener: Optional[EdgeEventListener], priority: bool = False) -> None`

Register a listener object to receive events. `None` does nothing. The wrapper
retains the Python target until removal, clearing or accepted disposal.

- **listener**: An instance of `EdgeEventListener` or a subclass, or `None`.
- **priority**: Insert with priority ordering (default `False`).

Duplicate object registrations are permitted; each has a distinct native proxy.
See [callback ordering and lifetime](index.md#callbacks-and-listeners).

### `remove_listener(listener: EdgeEventListener) -> None`

Unregister one matching object registration. **listener** is the Python object
previously passed to `add_listener()`. For duplicates, the newest priority
registration is removed first, otherwise the oldest normal registration.
Removal does not wait for already executing callbacks to finish.

### `on_change(cb: Optional[Callable[[list[IrSensor]], None]]) -> None`

Set the change function callback, replacing any previous callable.

- **cb**: Called as `cb(sensors)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `on_gap_detect(cb: Optional[Callable[[GapDirection], None]]) -> None`

Set the gap detect function callback, replacing any previous callable.

- **cb**: Called as `cb(gap_type)`; pass `None` to unregister this function callback.
  It returns `None`. A noncallable, non-`None` value raises `TypeError`.

For multiple handlers, use class-based listeners or an application dispatcher.
See [delayed-callback and exception behavior](index.md#callbacks-and-listeners).

### `init() -> int`

Initialize edge sensor control.

Must be called before enabling control or reading sensors.

**Returns**

- `int`:

```text
0: success
1: already initialized
-1: enable GPIO initialization failed
-2: drive GPIO initialization failed
-3: front-left GPIO initialization failed
-4: front-right GPIO initialization failed
-5: back-left GPIO initialization failed
-6: back-right GPIO initialization failed
```


### `clear_listeners() -> None`

Unregister all object listeners and function callbacks belonging to this component
and release their stored Python targets. This does not dispose hardware or wait
for callback drainage. Already executing callbacks may finish.

### `dispose() -> int`

Dispose edge sensor control and release resources. Accepted disposal also cleans
up this component's Python registrations.

**Returns**

- `int`:

```text
0: success
1: not initialized
-1..-6: GPIO release failed after drainage; Python callback cleanup accepted
-7: callback-context rejection; registrations retained
```


### `is_active() -> bool`

Check whether edge sensor control is initialized and active.

**Returns**

- `bool`:

```text
true if active, false otherwise.
```


### `enable_control() -> int`

Start the sensor listening thread.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already running - -1 : not initialized
```

**Notes**

- The initialize function enables the module on startup.


### `disable_control() -> int`

Stop the sensor listening thread.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : not running - -1 : not initialized
```


### `get_sensors() -> list[IrSensor]`

Return all current sensor records without filtering.

### `get_sensors(state: GpioState) -> list[IrSensor]`

Get IR sensors filtered by GPIO state.

**Parameters**

- **state**: Desired GPIO state to filter by. 0 = no ground detected (free) 1 = ground detected

**Returns**

- `list[IrSensor]`:

```text
List containing sensors matching the specified state.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

### `clear_callbacks() -> None`

Remove this component's function callbacks only, preserving object listeners.
Unlike `clear_listeners()`, this does not remove class-based registrations.
Neither operation waits for already executing callbacks to finish.

## Lifecycle contract

`init()` and `disable_control()` raise `RuntimeError` from any wrapper callback.
`dispose()` returns `-7` there without changing registrations. Native outcomes
`0`, `1` and post-drain GPIO errors `-1` through `-6` permit Python cleanup;
resource errors remain errors even when callback targets have been released.

See [initialization and disposal](index.md#initialization-and-disposal) for
component-local cleanup, retained native proxies and interpreter limitations.


