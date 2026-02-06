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

### `init(depth: LcdColorDepth = LcdColorDepth::L12BIT) -> int`

Initialize the LCD device.

**Parameters**

- **depth**: LCD color depth to configure. (default: `LcdColorDepth::L12BIT`)

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


### `write_lcd(side: LcdSide, buffer: py::buffer) -> int`

Write buffer into LCD memory.

Size depends on panel resolution and configured color depth. Example: 240x240x3 for 18-bit depth represented as 3 bytes/pixel.

**Parameters**

- **side**:
- **buffer**:

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

Get the currently configured LCD color depth.

**Returns**

- `LcdColorDepth`:

```text
Current color depth.
```


### `set_brightness(value: int) -> None`

Set LCD backlight brightness.

**Parameters**

- **value**: Brightness value (min = 0, max = 10).


### `to_lcd_buffer(input: py::buffer, input_rgba: bool = False) -> bytes`

Convert buffer to LCD buffer based on LCD depth.

**Parameters**

- **input**:
- **input_rgba**:  (default: `False`)

**Returns**

- `bytes`:

```text
Buffer array.
```


### `get_version() -> float`

Get current library version.

Original note: format 0.XYZ (3 digits after major).

**Returns**

- `float`:

```text
Version as float.
```
