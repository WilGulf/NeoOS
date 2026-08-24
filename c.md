[BACK](README.md)

# C in NeoOS

NeoOS has multiple C functions and systemcalls that is used in the programs available and possible for anyone to use in their own programs for NeoOS.

## Program layout

The Nexus kernel expects a *void declerations(void)* function present as the program execution starts there.'

All NeoOS programs are needed to make a promise of what capabilities it will have, these capabilities include: printing, get keyboard input and executing other programs. If a promise gets broken the kernel terminates the process.

After the declarations the program executes main like any other C program.

<details>
<summary><strong>List of promises</strong></summary>

**PROMISE_FB**: Provides access to framebuffer with commands like: printf, putchar and clear.

**PROMISE_INPUT**: Provides keyboard access.

**PROMISE_EXEC**: Provides access to system and fork commands.

**PROMISE_KILL**: Allows process to kill other processes.

</details>
<br>

### Example
```c
void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    exit(0);
}
```

## Syscalls

Syscalls are ways to interract with the system like printing or executing a program.

### Basic

- **printf(char *fmt, ...)**: prints the formatted string to the screen
- **putchar()**: Prints a single *char* to the screen
- **getkey()**: Returns a *char* representing a keypress, may return 0
- **getkey_event(&event)**: Fills the provided key_event struct with key char and eventual modifiers (ctrl, alt, shift)
- **exit()**: Exits the program and returns to the shell or parent process

### Memory

- **malloc()**: Basic C malloc
- **free()**: Basic C free

### Execute new processes

- **system(const char *command)**: Executes the command and doesn't return until program has exited
- **fork(const char *command)**: Executes the command and returns immediatly

## Security features

Every program starts with a privilege level set or inherited from the parent.
The privilege level is another factor that determines the programs capabilities and what parts of the filesystem the program can access.

No process are ever allowed to increase it privilege or it gets terminated. However dropping the level to a lower one is perfectly fine and is recommended if the program does nolonger need a as high privilege.

### Privilege commands in C

- **drop_privilege(uint8_t new_privilege)**: Drops the privilege to the new level

- **system_as(const char *command, uint8_t privilege)**: Executes the command with the specified privilege and doesn't return until the program has exited.

- **fork_as(const char *command, uint8_t privilege)**: Executes the command with the specified privilege and returns immediately.

## Nexus kernel interractions

Getting information about the running kernel can easily be done after including the kernel.h . That provides both the syscall and prototypes for the structs.

Example structs:
```c
struct kernel_version {
    char name[64];
    int version_major;
    int version_minor;
    int version_patch;
    char extension[16];
};

struct kernel_info {
    struct kernel_version version;
};
```

### Command

- **get_kernel(void *out_ptr)**: Writes kernel information from the kernel to the out pointer expected to point to a kernel_info struct.

## Other C commands

NeoOS header files provide many functions found in string.h and memory.h, with behavior matching their standard counterparts.