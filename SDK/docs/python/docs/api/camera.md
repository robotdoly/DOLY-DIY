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

## Classes

### `Options`

**Fields**

- **help**
- **version**
- **list_cameras**
- **verbose**
- **timeout**
- **photo_width**
- **photo_height**
- **video_width**
- **video_height**
- **roi_x**
- **roi_y**
- **roi_width**
- **roi_height**
- **shutter**
- **gain**
- **ev**
- **awb_gain_r**
- **awb_gain_b**
- **brightness**
- **contrast**
- **saturation**
- **sharpness**
- **framerate**
- **denoise**
- **info_text**
- **camera**

**Methods**

- `set_metering(mode: Any) -> None`
- `set_white_balance(mode: Any) -> None`
- `set_exposure_mode(mode: Any) -> None`
- `get_exposure_mode() -> None`
- `get_metering_mode() -> None`
- `get_white_balance() -> None`

### `PiCamera`

**Methods**

- `start_photo(self: PiCamera) -> bool`
- `capture_photo(self: PiCamera) -> py::object`
- `stop_photo(self: PiCamera) -> bool`
- `start_video(self: PiCamera) -> bool`
- `get_video_frame(timeout_ms: PiCamera = 1500) -> py::object`
- `stop_video(self: PiCamera) -> None`
- `apply_zoom_options(self: PiCamera) -> None`
- `set_exposure(value: PiCamera) -> None`
- `set_awb_enable(enable: PiCamera) -> None`

## Functions
