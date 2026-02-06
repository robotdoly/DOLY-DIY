# Color API reference

Import:

```python
import doly_color
```

This page documents the public API exposed by the `doly_color` Python module.

## Enums

### `ColorCode`

Values:

- `Black`
- `White`
- `Gray`
- `Salmon`
- `Red`
- `DarkRed`
- `Pink`
- `Orange`
- `Gold`
- `Yellow`
- `Purple`
- `Magenta`
- `Lime`
- `Green`
- `DarkGreen`
- `Cyan`
- `SkyBlue`
- `Blue`
- `DarkBlue`
- `Brown`

## Classes

### `Color`

**Fields**

- **r**: Return string representation.
- **g**: Return string representation.
- **b**: Return string representation.

**Methods**

- `toString() -> str`
  - Return string representation.
- `get_color(r: int, g: int, b: int) -> Color`
  - Create a Color from r,g,b.
- `hex_to_rgb(hex: str) -> Color`
  - Convert hex string to Color.
- `from_code(code: ColorCode) -> Color`
  - Get a Color from ColorCode.
- `get_led_color(code: ColorCode) -> Color`
  - Get closest color tone for LEDs.
- `get_color_name(code: ColorCode) -> str`
  - Get color name from ColorCode.

