# Install

Doly ships with the Python SDK **preinstalled** on the robot (Raspberry Pi OS, Python 3.11).

## Using the SDK (on the robot)

Most users can start immediately:

- Import the module (e.g., `import doly_arm`)
- Run the module `example.py` from the repository as a starting point
- Build your own script based on the examples

## Developer install (rebuild from source)

If you modify a module’s source code, rebuild and reinstall that module on the robot.

Typical workflow (per module):

1. Edit `source/bindings.cpp`
2. Build a wheel using the module’s `pyproject.toml`
3. Install the wheel on the robot using `pip`

> The exact build commands can differ based on your toolchain and where the C++ libraries live. We document the recommended build steps in each module’s repository and keep this page focused on end-user setup.
