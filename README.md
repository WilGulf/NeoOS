# NeoOS
A 32-bit Operating System for the x86 architecture.

## Features
Currently implemented:
- 32-bit x86 kernel
- Multiboot-compatible boot process
- Basic kernel initialization
- Early paging support

Planned features:
- Hardware drivers
- Filesystem support
- User programs
- UNIX compatible shell

## Download latest ISO

[Download NeoOS ISO](https://raw.githubusercontent.com/WilGulf/NeoOS/downloads/builds/neoos-latest.iso)

## Build and run it from source code
**Note:** Running the ISO with `make run` requires the x86 version of QEMU to be installed.
```bash
git clone https://github.com/WilGulf/NeoOS.git
cd NeoOS

#Building
make os.iso

#Running
make run