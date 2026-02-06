# FAQ

## Do I need to install anything on the robot?
No. The Python SDK is installed with the robot image.

## Can I modify the SDK source and rebuild?
Yes. Each module contains a `source/` folder with `pyproject.toml` and CMake configuration.

## Why does my script conflict with the Doly service?
If the service and your script try to access the same hardware at the same time, conflicts can happen.
Stop the `doly.service` before running scripts that directly control hardware. Please refer to the SDK examples for a demonstration. 
