# Permissions

Some modules talk to Linux device nodes (for example, `/dev/doly_lcd`) or sysfs entries.  
If a module works as **root** but not as a **user**, it is usually a permissions problem. We have already added the `doly` user to the custom `doly` group. If you plan to add a new user to your system to interact with Linux device nodes, please follow the instructions below.

## Recommended approach

- Use **udev rules** to assign a group and permissions to Doly devices
- Add your user to the appropriate group (often `video`, `dialout`, or a custom `doly` group)

