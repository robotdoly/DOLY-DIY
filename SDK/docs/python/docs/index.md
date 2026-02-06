# Python SDK

This section documents the Doly Python SDK.

## Overview

The Doly Python SDK is a collection of small, focused modules (one per capability) built on top of Doly’s C++ SDK using **pybind11**.

- **Platform**: Raspberry Pi OS
- **Python**: 3.11
- **Distribution**: preinstalled on the robot; developers can rebuild modules from source.

## Module layout (typical)

Each module usually contains:

- `example.py` — a minimal working usage example
- `source/` — build system (CMake + pyproject) and pybind11 bindings

## Documentation style

Each module page includes:

- Import instructions
- A minimal working example (from `example.py`)
- Common tasks / recipes
- Notes (permissions, services, gotchas)
- A link to the API reference page


## Links

 - [Doly Shop](https://shop.doly.ai)
 - [Doly Website](https://doly.ai)
 - [Doly Community](https://community.doly.ai)
 - [GitHub Repository](https://github.com/robotdoly/DOLY-DIY)
