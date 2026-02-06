# Helper API reference

Import:

```python
import doly_helper
```

This page documents the public API exposed by the `doly_helper` Python module.

## Classes

### `Position`

**Fields**

- **head**
- **x**
- **y**

### `Position2F`

**Fields**

- **x**
- **y**

## Functions

### `read_settings() -> int`

Read default settings from the platform settings file.

This is commonly used to load calibration/config values that the SDK uses at runtime.

**Returns**

- `int`:

```text
Status code: - >= 0 : success - -1 : settings file not found - -2 : XML open or parse error
```


### `get_imu_offsets(gx: int, gy: int, gz: int, ax: int, ay: int, az: int) -> int`

Retrieve IMU calibration offsets (gyro/accel).

Reads previously stored IMU offset values. These offsets are typically passed into DriveControl::init(...) to start with calibrated parameters.

**Parameters**

- **gx**: Gyro X offset.
- **gy**: Gyro Y offset.
- **gz**: Gyro Z offset.
- **ax**: Accel X offset.
- **ay**: Accel Y offset.
- **az**: Accel Z offset.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : failed to read offsets
```


### `stop_doly_service() -> int`

Stop the background Doly service (if running).

Some applications/tests may need exclusive access to hardware resources owned by the service. This helper provides a unified method to request stopping it.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : service not active - -1 : error while stopping service
```
