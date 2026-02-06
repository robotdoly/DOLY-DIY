# TtsControl API reference

Import:

```python
import doly_tts
```

This page documents the public API exposed by the `doly_tts` Python module.

## Enums

### `VoiceModel`

Values:

- `Model1`
- `Model2`
- `Model3`

## Functions

### `init(model: VoiceModel, output_path: str = "") -> int`

Initialize the TTS system.

**Parameters**

- **model**: Voice model to use.
- **output_path**: Optional output directory/path used by the implementation. (default: `""`)

**Returns**

- `int`:

```text
Status code: - 0 : success - 1 : already initialized - -1 : model file missing - -2 : model config file missing
```

**Notes**

- Model loading is a time-consuming and CPU-intensive operation; it is a blocking function.


### `dispose() -> int`

Dispose the TTS system and release resources.

**Returns**

- `int`:

```text
Status code (implementation-defined).
```


### `produce(text: str) -> int`

Produce a speech audio output from text.

**Parameters**

- **text**: Input text to synthesize.

**Returns**

- `int`:

```text
Status code: - 0 : success - -1 : TTS not active (init() not called or failed) - -2 : data processing error
```

**Notes**

- This operation blocking the thread.


### `get_version() -> float`

Get the TTS subsystem/library version.

**Returns**

- `float`:

```text
Version number.
```
