#define KERNEL_NAME "Nexus"
#define KERNEL_VERSION_MAJOR 1
#define KERNEL_VERSION_MINOR 0
#define KERNEL_VERSION_PATCH 0
#define KERNEL_FLAVOUR ""


#include "kernel.h"

#include "include/stdint.h"

#include "multiboot.h"
#include "drivers/io/io.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "memory/paging.h"
#include "memory/kheap.h"
#include "memory/heap.h"
#include "task/task.h"
#include "task/process.h"
#include "syscalls/isr80h.h"
#include "drivers/fs/disk.h"
#include "drivers/fs/path_parser.h"
#include "drivers/keyboard/keyboard.h"
#include "include/util.h"
#include "include/status.h"
#include "include/va_list.h"
#include "panic.h"

extern uint32_t kernel_virtual_start;
extern uint32_t kernel_virtual_end;
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;
extern uint32_t stack_top;

static struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_page() {
    kernel_registers();
    paging_switch(kernel_chunk);
}

static struct kernel_version kernel_version = {
    .name = KERNEL_NAME,
    .version_major = KERNEL_VERSION_MAJOR,
    .version_minor = KERNEL_VERSION_MINOR,
    .version_patch = KERNEL_VERSION_PATCH,
    .extension = KERNEL_FLAVOUR,
};

struct kernel_version *get_kernel_version() {
    return &kernel_version;
}

int kmain(uint32_t magic, struct multiboot_info* bootInfo) {    
    fb_clear();

    kheap_init();
    kprintf("Inititalized kernel heap\n");

    disk_search_and_init();

    gdt_init();
    kprintf("GDT Initialized\n");
    idt_init();
    kprintf("IDT Initialized\n");

    void *kernel_stack = kzalloc(4096);
    set_tss_stack(&stack_top);

    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    kprintf("Paging new\n");
    
    paging_switch(kernel_chunk);
    kprintf("Paging switch\n");

    enable_paging();
    kprintf("Paging enabled\n");

    isr80h_register_commands();

    fs_init();
    disk_search_and_init();

    keyboard_init();

    struct process *process = 0;
    int res = process_load_switch("0:/sysro/launch.elf", &process);
    if (res != ALL_OK) {
        panic("Failed to load init", res);
    }

    task_run_first_ever_task();

    while (1) {}

    return 0;
}