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

`Color()` creates a color with all channels initially zero. The shared Python type
is reused by Eye and LED. Channels are writable integers in 0–255.

**Fields**

- **r: int**: Red channel, default 0.
- **g: int**: Green channel, default 0.
- **b: int**: Blue channel, default 0.

**Instance methods**

- `toString() -> str`
  - Return string representation.

`__repr__() -> str` produces `Color(r=…, g=…, b=…)`.

**Static methods**

- `get_color(r: int, g: int, b: int) -> Color`
  - Create a Color from r,g,b.
- `hex_to_rgb(hex: str) -> Color`
  - Convert a hex string to Color using the native parser. Malformed input may
    raise translated exceptions such as `ValueError` or `IndexError`; the binding
    does not enforce strict six-digit syntax or guarantee a black fallback.
- `from_code(code: ColorCode) -> Color`
  - Get a Color from ColorCode.
- `get_led_color(code: ColorCode) -> Color`
  - Get closest color tone for LEDs.
- `get_color_name(code: ColorCode) -> str`
  - Get color name from ColorCode.


