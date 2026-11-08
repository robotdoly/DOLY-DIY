# Helper API reference

Import:

```python
import doly_helper
```

This page documents the public API exposed by the `doly_helper` Python module.

## Classes

### `Position`

`Position()` creates a position record. Fields are writable. Drive returns this
shared type from `get_position()`.

**Fields**

- **head: float**: Heading in degrees for positions returned by Drive.
- **x: int**: X position in millimetres for positions returned by Drive.
- **y: int**: Y position in millimetres for positions returned by Drive.

### `Position2F`

`Position2F()` creates a two-dimensional position record. Fields are writable.

**Fields**

- **x: float**: X position.
- **y: float**: Y position.

## Functions

### `read_settings() -> int`

Read default settings from the platform settings file.

This is commonly used to load calibration/config values that the SDK uses at runtime.

**Returns**

- `int`:

```text
Status code: - >= 0 : success - -1 : settings file not found - -2 : XML open or parse error
```


### `get_imu_offsets() -> tuple[int, int, int, int, int, int, int]`

Retrieve previously stored IMU calibration offsets (gyro/accel). This function takes
no arguments. These offsets are typically passed into `doly_drive.init(...)` or
`doly_imu.init(...)` to start with calibrated parameters.

**Returns**

`(result, gx, gy, gz, ax, ay, az)`: status followed by gyro X/Y/Z and accelerometer
X/Y/Z offsets. `result` is `0` on success or `-1` on failure. Check it before using
the six offsets; output placeholders on failure are not valid calibration data.

### `stop_doly_service() -> int`

Stop the background Doly service (if running).

Some applications/tests may need exclusive access to hardware resources owned by the service. This helper provides a unified method to request stopping it.

**Exceptions and effects**

This affects other applications using the background service. It is an explicit
operation, not automatic example setup. From a wrapper callback it raises
`RuntimeError`; perform service/control work on the application control thread.
There is no public `close_callbacks()` helper.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : service not active - -1 : error while stopping service
```


