# VContent API reference

Import:

```python
import doly_vcontent
```

This page documents the public API exposed by the `doly_vcontent` Python module.

## Classes

### `VContent`

**Fields**

- **active_frame_id**: Selected frame on load.
- **ft**: Total frames in sequence.
- **width**: Frame width.
- **height**: Frame height.
- **path**: Source path.
- **alpha**: True if content has alpha channel.
- **color12Bit**: True if content is 12-bit color (or more).
- **ratio**: Frame ratio divider.
- **loop**: Loop count (0 = forever).
- **frames**: Raw frames as list of byte arrays (debug/advanced).

**Methods**

- `__init__() -> None`
  - Create an empty VContent.
- `is_ready() -> bool`
  - Return True if the content has been loaded successfully.
- `get_frame_bytes(index: int = 0) -> bytes`
  - Get a frame as bytes (raw RGB/RGBA buffer as stored by the library).
- `get_image(path: str, isRGBA: bool, set12Bit: bool) -> VContent`
  - Load a PNG from path and return a VContent.
    isRGBA: True if loaded image has alpha.
    set12Bit: convert to 12-bit color depth.

