# Python API reference

Reference for the current Python module families. These pages describe the
maintained Python interfaces, including adapters, properties and callback helpers.


## Signatures and return values

Use the Python names and defaults on each page. A native integer status, Python
exception, boolean result and `None` are different contracts. Check the documented
status before using tuple payloads. In particular, Drive disposal returns `None`,
and Helper/IMU calibration adapters return a status followed by six offsets.

`Callable` and `Optional` in signatures describe Python callables and values that
may be `None`; they are typing notation, not SDK classes. Listener method parameter
names describe positional event payloads unless keyword support is explicitly
documented. Native integer parameters have their native signed/unsigned width;
out-of-range or incompatible arguments can fail Python conversion rather than
returning a native error status. Only explicitly documented adapters clamp inputs.

Shared types have one owner: `Position` in `doly_helper`, `Color` in `doly_color`,
`GpioState` in `doly_io`, and `VContent` in `doly_vcontent`. Other modules reuse these
types. Do not construct unexposed native types or call native destructors directly.

## Callbacks and listeners

`add_listener(listener, priority=False)` accepts the component's listener type.
`add_listener(None)` does nothing. The wrapper retains registered Python targets;
dropping an application reference does not unregister a listener. Drive and Battery
deduplicate the same object. Other callback components permit duplicate object
registrations. `remove_listener(listener)` removes one matching registration:
the newest priority registration first, otherwise the oldest normal registration.

Within one event dispatch, priority object listeners run first, newest priority
registration first. Normal object listeners follow in registration order. This
does not establish ordering between separate events.

Each `on_*` setter accepts one callable or `None`. A callable installs or replaces
that event's function callback; `None` removes it. Noncallable values raise
`TypeError`. Most setters use keyword `cb`; Battery uses `callback`. Object listeners
are dispatched before function callbacks in native event ordering. Callbacks run
through native dispatch with the Python GIL, not through an application event queue.
Exceptions from callbacks are reported as unraisable and do not escape into native
event dispatch. Keep callbacks short and schedule blocking control work on the
application control thread.

`clear_listeners()` removes both object and function registrations for that
component. It does not dispose hardware or wait for callbacks to drain. Edge also
provides `clear_callbacks()`, which removes only function callbacks. Explicit
removal and self-removal do not wait for already executing callbacks to finish;
native registry removal may briefly acquire its own mutex.

An event admitted before a function callback is removed can reach its replacement
after re-registration. Removing and re-registering a function callback does not
isolate old events from new handlers. For frequent replacement, prefer class-based
listeners. An already executing callback may still finish; class listeners do not
provide a complete shutdown or drainage guarantee.

## Initialization and disposal

Use each component's existing `dispose()` on the application control thread before
interpreter finalization. There is no public global callback shutdown function and
no `doly_helper.close_callbacks()`. Callback cleanup affects only that component;
Drive's native `dispose_IMU` option does not clear Python IMU registrations.

Initialization and blocking operations identified on each page raise `RuntimeError`
when called from any wrapper callback. Disposal adapters with known rejection codes
return those codes instead. Drive preserves its `None` adapter; Eye performs only
callback cleanup. New registration during disposal/finalization raises `RuntimeError`.

Disposal pauses callback admission. Suppressed events are not queued or replayed.
Rejected native disposal retains surviving registration identities, priorities,
ordering and Python targets. Explicit removals made during the attempt stay removed.
Accepted cleanup unregisters that component and releases Python target references
after admitted Python entries finish. An executing callback holds a local target
reference. Native disposal can block indefinitely; a status does not universally
prove native worker drainage.

Each newly created object registration retains a native proxy until process
termination, even after accepted cleanup. Removing and registering again creates
another retained proxy; no reclamation or bounded-memory guarantee is made.
`add_listener(None)` and ignored duplicate registrations in Battery/Drive return
before allocation. Function callback setters reuse one subscription per event.
Ambiguous Sound/IMU failures may retain Python targets until explicit removal or
later accepted disposal.

## Ownership and limitations

[Camera](camera.md) owns its options; returned frames own their Python array storage.
[VContent](vcontent.md) shares native ownership with [Eye](eye_control.md), which
retains final image owners because no supported detach-and-join API exists.
[LCD](lcd_control.md) retains exported buffers through synchronous native access.
Follow the image dimensions, idle-state, initialization and buffer requirements on
those pages. Do not operate LCD and Eye concurrently.

Wrapper coordination does not synchronize external C++ callers or repair native
rendering/shutdown races. Interpreter exit does not dispose native devices or
guarantee safe shutdown; dispose components on the control thread before
finalization. Embedded interpreter restart, subinterpreters, free-threaded Python,
dynamic unloading and native entry into Python during finalization are unsupported.
See [installation and compatibility](../getting-started/install.md) for interpreter
and platform requirements.

## Usage demonstrations

Each component API page links to its module guide, which includes the full example
and its source path under `examples/python/` in the SDK distribution. Color and
VContent refer to the LED/Eye guides. Follow each guide's device, asset, and cleanup
requirements. **The Battery example is for reading only and must never execute.**
