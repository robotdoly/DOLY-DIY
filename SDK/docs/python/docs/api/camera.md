# Camera API reference

Import:

```python
import doly_camera
```

This page documents the public API exposed by the `doly_camera` Python module.

## Enums

### `ExposureModes`

Values:

- `EXPOSURE_NORMAL`
- `EXPOSURE_SHORT`
- `EXPOSURE_CUSTOM`

### `MeteringModes`

Values:

- `METERING_CENTRE`
- `METERING_SPOT`
- `METERING_MATRIX`
- `METERING_CUSTOM`

### `WhiteBalanceModes`

Values:

- `WB_AUTO`
- `WB_NORMAL`
- `WB_INCANDESCENT`
- `WB_TUNGSTEN`
- `WB_FLUORESCENT`
- `WB_INDOOR`
- `WB_DAYLIGHT`
- `WB_CLOUDY`
- `WB_CUSTOM`

The listed exposure, metering, and white-balance enum members are also available
directly on `doly_camera`, for example `doly_camera.EXPOSURE_NORMAL`.

## Classes

### `Options`

Obtain this object from `camera.options` on a `PiCamera` instance. There is no
standalone Python `Options()` constructor. The options reference retains its
owning camera; the `options` property cannot be replaced, but the fields below
are writable. Configure options before starting the corresponding capture mode.


**Fields**

- **help: bool**
- **version: bool**
- **list_cameras: bool**
- **verbose: bool**
- **timeout: int**
- **photo_width: int**
- **photo_height: int**
- **video_width: int**
- **video_height: int**
- **roi_x: float**
- **roi_y: float**
- **roi_width: float**
- **roi_height: float**
- **shutter: float**
- **gain: float**
- **ev: float**
- **awb_gain_r: float**
- **awb_gain_b: float**
- **brightness: float**
- **contrast: float**
- **saturation: float**
- **sharpness: float**
- **framerate: float**
- **denoise: str**
- **info_text: str**
- **camera: int**

**Methods**

- `set_metering(mode: MeteringModes) -> None`
- `set_white_balance(mode: WhiteBalanceModes) -> None`
- `set_exposure_mode(mode: ExposureModes) -> None`
- `get_exposure_mode() -> int`
- `get_metering_mode() -> int`
- `get_white_balance() -> int`
- `print() -> None`

The three setters update the corresponding native mode setting. The getters
return integer mode values. `print()` prints native option information.
`timeout` is milliseconds; dimensions are pixels; `framerate` is frames per second.

### `PiCamera`

`PiCamera()` constructs a camera. Its read-only `options: Options` property exposes
the fields above. Important effective constructor defaults are:

| Option | Default |
|---|---|
| Photo dimensions | 3280 × 2464 |
| Video dimensions | 640 × 480 |
| `framerate` | 30 |
| `timeout` | 1000 ms |
| Metering | `MeteringModes.METERING_MATRIX` |
| Exposure | `ExposureModes.EXPOSURE_NORMAL` |
| White balance | `WhiteBalanceModes.WB_AUTO` |
| `denoise` | `"auto"` |
| `contrast`, `saturation`, `sharpness` | 1.0 |
| `brightness`, `shutter`, `gain`, `ev` | 0.0 |
| ROI coordinates/dimensions and AWB gains | 0.0 |
| `camera` | 0 |
| `verbose` | `False` |

Other fields have no promised default here. Example-selected values are not
constructor defaults.


**Methods**

- `start_photo() -> bool`
- `capture_photo() -> Optional[numpy.ndarray]`
- `stop_photo() -> bool`
- `start_video() -> bool`
- `get_video_frame(timeout_ms: int = 1500) -> Optional[numpy.ndarray]`
- `stop_video() -> None`
- `apply_zoom_options() -> None`
- `set_exposure(value: float) -> None`
- `set_awb_enable(enable: bool) -> None`

`start_photo()` and `start_video()` report whether startup succeeded. Use
`capture_photo()` only after photo startup and `get_video_frame()` after video
startup. `stop_photo()` returns a boolean, whereas `stop_video()` returns `None`.
Keep explicit cleanup, including when capture or application processing fails.
`apply_zoom_options()` applies the configured ROI; `set_exposure(value)` and
`set_awb_enable(enable)` forward the native controls. There is no blanket promise
that configuration/capture failures are converted into status results: native
exceptions may propagate. Releasing the GIL during native calls does not imply
that callers may safely mutate configuration concurrently.

## Frame results and ownership

Capture failure or an empty native frame returns `None`. Supported successful
frames are independent Python-owned NumPy arrays with `uint8` storage:

- Grayscale: `(height, width)`.
- Three/four channels: `(height, width, channels)`.

The adapter copies rows, including from non-contiguous native storage; keeping
an array does not retain a borrowed native frame buffer. An unexpected native
channel count returns an empty NumPy array, so non-`None` alone does not guarantee
a usable image. The adapter preserves native channel ordering.

`get_video_frame(timeout_ms=1500)` uses milliseconds and an unsigned integer
timeout. That default is separate from `options.timeout`.

Only the classes, fields and methods listed here are exposed. Internal
`LibcameraApp` types, native-only controls/fields and explicit destructor APIs
are not supported Python interfaces. There is no camera context-manager API.


