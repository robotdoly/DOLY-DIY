# BatteryControl API reference

Import:

```python
import doly_battery
```

This page documents the public API exposed by the `doly_battery` Python module:
battery monitoring, capacity and alarm callbacks. Initialize monitoring before
using readings. Capacity is an estimate in percent, not a measurement of true
state of charge or a calibration guarantee.

## Classes

### `BatteryEventListener`

`BatteryEventListener()` constructs the listener base. Subclass it and register
the instance with `add_listener()` to receive alarms.

**Methods**

- `on_battery_alarm(capacity: int) -> None`
  - Called with capacity in percent when the battery alarm is delivered.
    The bound method supports the keyword `capacity`.

The proxy also recognizes a legacy Python `onBatteryAlarm` override. Override-name
selection occurs once at registration. This is not an additional bound base-class
method; prefer `on_battery_alarm` in new subclasses.

## Functions

### `add_listener(listener: Optional[BatteryEventListener], priority: bool = False) -> None`

Register a listener instance. The wrapper retains its Python target until removal,
clearing or accepted component disposal. `None` does nothing. Registering the same
object again is ignored; it does not change the original registration's priority.

- **listener**: A BatteryEventListener instance/subclass, or `None`.
- **priority**: Insert with priority ordering, default `False`.

### `remove_listener(listener: Optional[BatteryEventListener]) -> None`

Unregister the given listener object. `None` does nothing. Removal does not wait
for an already executing callback to finish.

### `on_alarm(callback: Optional[Callable[[int], None]]) -> None`

Install or replace the single alarm function callback. It is called as
`callback(capacity)` and returns `None`. Pass `None` to unregister it. The keyword
is `callback`, not `cb`; a noncallable, non-`None` value raises `TypeError`.

See [callback ordering, exceptions and delayed delivery](index.md#callbacks-and-listeners).

### `clear_listeners() -> None`

Unregister this component's object listeners and function callback and release
their stored Python targets. This neither disposes battery monitoring nor waits
for callback drainage. An already executing callback may finish.

### `init() -> int`

Initialize battery monitoring.

**Returns**

```text
0: success
1: already active or lifecycle operation in progress
-1: sensor path unavailable
```

From any wrapper callback this raises `RuntimeError`; initialize on the
application control thread.

### `dispose() -> int`

Dispose battery monitoring and release resources. Native `0/1` permit cleanup of
Battery Python subscriptions. Native `-1` retains surviving registrations.

**Returns**

```text
0: success
1: already inactive, including after waiting for another disposal to complete
-1: rejected disposal, including callback context; registrations retained
```

Call on the application control thread. Accepted Python cleanup is not proof of
universal native drainage or bounded disposal time.

### `is_active() -> bool`

Return whether battery monitoring is initialized and active.

### `set_alarm_threshold(value: int) -> None`

An alarm may be delivered when the capacity estimate is below the configured
threshold (`capacity < threshold`). The native alarm repeat interval is ten seconds.
This does not guarantee that an event will occur during a particular observation
period.

**value** is forwarded as an unsigned 8-bit integer (0–255). Use 0–100 for an
ordinary percentage threshold. The wrapper does not clamp values to 0–100;
values outside the native integer range fail conversion.

### `get_capacity() -> int`

Return the current capacity estimate in percent. Read after successful
initialization. The reported 0–100 estimate does not establish physical current
accuracy, sensor calibration or true state of charge.

### `get_version() -> float`

Return the current library version as a float.

## Lifecycle and supported scope

See [initialization and disposal](index.md#initialization-and-disposal) for
registration rejection during disposal, callback target retention, resident proxy
shells and interpreter limitations. Native initialization rollback, unchecked
sensor reads, retained filter/status state and potentially blocking disposal remain
limitations; the Python interface does not establish complete lifecycle safety.

`BatteryStatus`, status callbacks/delivery, `get_status`, `get_charge_time`,
current/voltage readings and internal event lifecycle/dispatch functions are not
exposed Python features.


