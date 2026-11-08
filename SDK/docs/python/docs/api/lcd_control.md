# LcdControl API reference

Import:

```python
import doly_lcd
```

This page documents the public API exposed by the `doly_lcd` Python module.

## Enums

### `LcdColorDepth`

Values:

- `L12BIT`
- `L18BIT`

### `LcdSide`

Values:

- `Left`
- `Right`

## Functions

### `init(depth: LcdColorDepth = LcdColorDepth.L12BIT) -> int`

Initialize the LCD device.

**Parameters**

- **depth**: LCD color depth to configure. (default: `LcdColorDepth.L12BIT`)

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized - -1 : open device failed - -2 : ioctl failed
```


### `dispose() -> int`

Release resources and deinitialize the LCD device.

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already closed or not opened
```


### `is_active() -> bool`

Check whether the LCD subsystem is active (initialized).

**Returns**

- `bool`:

```text
true if initialized; false otherwise.
```


### `lcd_color_fill(side: LcdSide, r: int, g: int, b: int) -> None`

Fill a panel with a solid RGB color.

**Parameters**

- **side**: LCD side to fill.
- **r**: Red component (0..255).
- **g**: Green component (0..255).
- **b**: Blue component (0..255).


### `write_lcd(side: LcdSide, buffer) -> int`

Write buffer into LCD memory.

Size depends on panel resolution and configured color depth. Example: 240x240x3 for 18-bit depth represented as 3 bytes/pixel.

**Parameters**

- **side**: Target LCD panel.
- **buffer**: A C-contiguous byte buffer containing at least `get_buffer_size()`
  bytes in the configured native LCD layout; see buffer requirements below.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : ioctl error - -2 : not active
```


### `get_buffer_size() -> int`

Get required buffer size in bytes for one full frame.

Size depends on panel resolution and configured color depth. Example: 240x240x3 for 18-bit depth represented as 3 bytes/pixel.

**Returns**

- `int`:

```text
Buffer size in bytes.
```


### `get_color_depth() -> LcdColorDepth`

Get the currently configured LCD color depth. Raises `RuntimeError` before
initialization. Native driver/depth-read failures remain native limitations.

**Returns**

- `LcdColorDepth`:

```text
Current color depth.
```


### `set_brightness(value: int) -> int`

Set LCD backlight brightness.

**Parameters**

- **value**: Brightness value (min = 0, max = 10). Native unsigned 8-bit
  conversion applies; values greater than 10 within that range are clamped to 10.

**Returns**

`0` from the native setter. This result does not verify the physical backlight
state or successful driver/file access.


### `to_lcd_buffer(input, input_rgba: bool = False) -> bytes`

Convert buffer to LCD buffer based on LCD depth.

**Parameters**

- **input**: C-contiguous bytes describing a 240×240 RGB image, or RGBA when
  requested. Required byte count is 172,800 for RGB or 230,400 for RGBA.
- **input_rgba**: Whether each input pixel has four channels (default `False`).
  Alpha is discarded in RGBA conversion.

**Returns**

- `bytes`:

```text
Newly owned bytes of the native output buffer size.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```

## Buffer requirements and device ownership

`write_lcd` and `to_lcd_buffer` accept buffer-protocol objects, such as `bytes`,
`bytearray`, compatible `memoryview` objects and contiguous NumPy byte arrays.
There is no SDK `Buffer` class. Element size must be one byte with buffer format
`B`, `b` or `c`. Storage must be C-contiguous and at least the required length;
strided views and arbitrary numeric arrays are not accepted. Conversion interprets
the contiguous input as a 240×240 RGB/RGBA layout; a particular array shape is not
required if the byte layout and size are correct.

| Layout | Required bytes |
|---|---:|
| 240×240 RGB conversion input | 172,800 |
| 240×240 RGBA conversion input | 230,400 |
| Native 12-bit LCD output | 86,400 |
| Native 18-bit LCD output | 172,800 |

Use `get_buffer_size()` for the actual native output requirement. Conversion
allocates that output size. Wrong element format raises `TypeError`;
non-contiguous, undersized, overflowing buffers or invalid native size/layout
raise `ValueError`. `to_lcd_buffer()` and `get_color_depth()` raise `RuntimeError`
before initialization. A correctly sized buffer does not turn native device
failures into Python validation errors.

Initialize before LCD device operations and dispose on the application control
thread. Buffer exports remain alive through synchronous native access, with the
GIL held. External native threads must not mutate storage or change LCD driver
configuration during use. Do not operate LCD and Eye concurrently. Native driver
failures, including unchecked depth ioctl results, remain limitations; buffer
validation is not a driver-safety guarantee.
