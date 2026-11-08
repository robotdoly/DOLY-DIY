# VContent API reference

Import:

```python
import doly_vcontent
```

This page documents the public API exposed by the `doly_vcontent` Python module.

## Classes

### `VContent`

`VContent()` creates empty content with native shared ownership.

**Read-only metadata**

- **active_frame_id: int**: Current active-frame index; read-only from Python.
- **ft: int**: Total frames in sequence.
- **width: int**: Frame width.
- **height: int**: Frame height.
- **path: str**: Source path.
- **alpha: bool**: True if content has alpha channel.
- **color12Bit: bool**: True if content uses converted 12-bit color.
- **ratio: int**: Frame-rate divider (for example, 2 divides the base frame rate by two).
- **loop: int**: Loop count (0 = forever).
- **frame_count: int**: Actual number of stored frames (`len(frames)`).

**Frames property**

`frames` reads as `list[list[int]]`, with each inner list containing byte values
0–255. Reading it returns converted copies, not a live view of native storage.
Editing a returned list does not modify native frames: assign the complete
replacement through `content.frames = replacement`. Assignment does not update
read-only metadata or readiness. `frame_count` reflects the stored vector size;
`ft` remains the loaded sequence metadata.


**Methods**

- `__init__() -> None`
  - Create an empty VContent.
- `is_ready() -> bool`
  - Return True if the content has been loaded successfully.
- `get_frame_bytes(index: int = 0) -> bytes`
  - Return an independent bytes copy of the stored RGB/RGBA frame. An index
    outside the stored frame vector raises `IndexError`; negative integers fail
    unsigned-index conversion.
- `get_image(path: str, isRGBA: bool, set12Bit: bool) -> VContent`
  - Static method. Load a PNG from **path** and return content. All three arguments
    are required. **isRGBA** selects RGBA (`True`) or RGB (`False`) output;
    **set12Bit** requests 12-bit color conversion: each RGB channel is divided
    by 16 to yield 0..15; alpha remains unchanged at 0..255. Channels still occupy
    separate bytes, so conversion does not change the buffer length. Loading/decoding failure
    produces unready content; check `is_ready()` rather than assuming a missing
    file raises `FileNotFoundError`.



## Eye ownership and restrictions

See [Eye image requirements](eye_control.md#image-ownership-and-restrictions).
Eye retains a native shared owner for every assigned slot/side, so deleting a
Python reference does not free an image still attached to Eye. Final owners remain
resident because there is no supported native detach-and-join API; Eye callback
disposal does not release them.

Replacing `frames` on attached content requires idle Eye processing. Validation
checks role, dimensions, channel layout, active-frame index, converted-color
metadata, and every frame's byte length. It does not check individual RGB values:
the caller must keep RGB in 0..15 and alpha in 0..255 for converted Eye content.
Invalid structural properties or metadata raise `ValueError`.
Reading `active_frame_id` on attached content also requires idle Eye processing.
Busy or uncertain ownership state raises `RuntimeError`; callback callers use a
nonblocking lock attempt. These guards do not synchronize external native callers.
Do not mutate attached content through C++ while using Python wrappers.

Standalone frame assignment is not an image-loading or metadata-update operation.


