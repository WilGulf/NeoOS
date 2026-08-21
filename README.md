# NeoOS
A 32-bit hobby Operating System for the x86 architecture.

## Features
Currently implemented:
- 32-bit x86 kernel
- Filesystem support
- A basic shell with a set of command
- Multiple programs runnable from the shell
- Security features inspired from open-bsd

### The shell

The neo shell (nsh) is a basic implementation of a shell. The standard programs are not UNIX-compatible but some are explained [here](commands.md).

### NeoOS standard C library

The libraries and header files available in C are mostly commands close to what is found in normal C programs, but NeoOS includes some kernel interractions and security features which are explained [here](c.md)

## Testing NeoOS

> **NOTE** NeoOS is not a OS intended to be used on real hardware and recommands being run in a vm. NeoOS may run on some x86 cpu but it has not been tested yet.

First, make sure QEMU is installed. The package name may differ depending on your operating system:

- Brew: `brew install qemu`
- Apt: `apt install qemu-system-x86`
- Dnf: `dnf install qemu-system-x86`
- Pacman: `pacman -S qemu-system-x86`

### Running

If you already have `kernel.elf` and `disk.img`, place them in the same directory and run:

```bash
qemu-system-i386 -kernel kernel.elf -hda disk.img
```

## The Nexus kernel

Nexus is a single core multi-threadded kernel written for the 32-bit x86 architecture (i386). The kernel includes drivers needed for basic system interraction like keyboard input and text output.

## How it works

At the start of the computer/virtual machine the kernel are the first thing loaded and executed. The kernel sets up memory and drivers before loading a disk and executing the init system. From the init creating a fork that is the shell which later creates more programs. This then creates a tree of processes all coming from the init.