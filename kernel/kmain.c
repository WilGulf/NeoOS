//-----------------------//
//      KERNEL INFO      //
#define KERNEL_NAME "Nexus"
#define KERNEL_VERSION_MAJOR 26
#define KERNEL_VERSION_MINOR 0
#define KERNEL_VERSION_PATCH 0
#define KERNEL_FLAVOUR ""
//-----------------------//

#include "kernel.h"

#include "include/stdint.h"
#include "include/util.h"
#include "include/status.h"
#include "include/va_list.h"
#include "drivers/io/io.h"
#include "memory/kheap.h"
#include "memory/heap.h"
#include "memory/paging.h"
#include "memory/memory.h"
#include "panic.h"
#include "timer/timer.h"
#include "idt/idt.h"
#include "idt/irq.h"
#include "gdt/gdt.h"
#include "task/task.h"
#include "task/process.h"
#include "syscalls/isr80h.h"
#include "drivers/disk/disk.h"
#include "drivers/disk/gpt.h"
#include "drivers/fs/path_parser.h"

/*
#include "drivers/keyboard/keyboard.h"
*/

extern uint32_t kernel_virtual_start;
extern uint32_t kernel_virtual_end;
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;
extern uint32_t stack_top;

struct paging_descriptor *kernel_paging_descriptor = 0;
struct tss tss;
extern struct gdt_entry gdt[]; 

void kernel_page() {
    kernel_registers();
    paging_switch(kernel_paging_descriptor);
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

struct paging_descriptor *kernel_descriptor() {
    return kernel_paging_descriptor;
}

int kmain() {
    fb_clear();
    kprintf("\n");
    
    kheap_init();
    kprintf("Inititalized kernel heap\n");

    kernel_paging_descriptor = paging_descriptor_new(PAGING_MAP_LEVEL_4);
    if (!kernel_paging_descriptor) {
        panic("Failed to create kernel paging descriptor\n", 1);
    }
    paging_map_e820_memory_regions(kernel_paging_descriptor);
    paging_switch(kernel_paging_descriptor);
    kprintf("Paging inititalized\n");

    kheap_post_paging();

    idt_init();

    irq_enable(IRQ_TIMER);
    timer_init();

    keyboard_init();

    void *mb_stack_tss_end = kzalloc(0x100000);
    void *mb_stack_tss_begin = (void *)(((uintptr_t)mb_stack_tss_end) + 0x100000);
    paging_map(kernel_paging_descriptor, mb_stack_tss_end, mb_stack_tss_end, 0);
    
    memset(&tss, 0x00, sizeof(tss));
    tss.rsp0 = (uint64_t)mb_stack_tss_begin;
    tss.iopb_offset = sizeof(tss);

    struct tss_descriptor *tss_desc = (struct tss_descriptor *)&gdt[7];
    gdt_set_tss(tss_desc, &tss, sizeof(tss) - 1, 0x89, 0x00);
    
    tss_load(0x38);

    isr80h_register_commands();

    ramdisk_init();
    fs_init();

    kprintf("Found %d disks\n", disks_search_and_init());
    gpt_init();

    kprintf("Handing over to launcher\n");
    int fd = fopen("1:/sysro/boot.cfg", "r");
    if (fd < 0) {
        panic("Could not open launcher", 1);
    }

    char line[256];
    char launcher[MAX_PATH];
    while (fgets(line, sizeof(line), fd)) {
        if (!strncmp(line, "LAUNCHER=", 9)) {
            strncpy(launcher, line + 9, sizeof(launcher));
        }
    }
    fclose(fd);

    struct process *process = 0;
    int res = process_load_switch(launcher, &process);
    if (res != ALL_OK) {
        panic("Failed to load launcher", res);
    }

    task_run_first_ever_task();

    while (1) {}

    return 0;
}