#include "task.h"
#include "process.h"
#include "formats/elf_loader.h"

#include "../drivers/io/io.h"

#include "../drivers/fs/file.h"
#include "../include/status.h"
#include "../include/util.h"
#include "../memory/kheap.h"
#include "../memory/paging.h"
#include "../panic.h"

struct process *current_process = 0;
struct process *input_dest_process = 0;

static struct process *processes[MAX_PROCESSES] = {};

static void process_init(struct process *process) {
    memset(process, 0, sizeof(struct process));
}

static int process_terminate_allocations(struct process *process) {
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++) {
        if (process->allocations[i].ptr) {
            process_free(process, process->allocations[i].ptr);
        }
    }

    return 0;
}

int process_free_binary_data(struct process *process) {
    if (process->ptr) {
        kfree(process->ptr);
    }

    return 0;
}

int process_free_elf_data(struct process *process) {
    if (process->elf_file) {
        elf_close(process->elf_file);
    }
    return 0;
}

int process_free_program_data(struct process *process) {
    int res = 0;
    switch(process->filetype) {
        case PROCESS_FILETYPE_BINARY:
            res = process_free_binary_data(process);
            break;
        
        case PROCESS_FILETYPE_ELF:
            res = process_free_elf_data(process);
            break;

        default:
            res = -ERROR_INVALID_FORMAT;
            break;
    }

    return res;
}

void process_switch_to_any() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i]) {
            process_switch(processes[i]);
            return;
        }
    }

    panic("No processes to switch to", 0);
}

static void process_unlink(struct process *process) {
    processes[process->id] = 0x00;
    if (current_process == process) {
        process_switch_to_any();
    }
}

void process_memory_free(struct process *process) {
    process_terminate_allocations(process);
    process_free_program_data(process);

    if (process->stack) {
        kfree(process->stack);
        process->stack = NULL;
    }
    
    if (process->task) {
        task_free(process->task);
        process->task = NULL;
    }

    kfree(process);
}

int process_terminate(struct process *process) {    
    if (process->first_child) {
        process_terminate(process->first_child);
    }
    if (process->sibling_child) {
        process_terminate(process->sibling_child);
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {
        struct process *process2 = process_get(i);
        if (process != NULL) {
            if (process2->task->parent == process->task) {
                process2->task->parent = NULL;
            }
        }
    }

    process_unlink(process);
    process_memory_free(process);

    return 0;
}

struct process *process_current() {
    return current_process;
}

struct process *get_input_process() {
    return input_dest_process;
}

int input_dest_process_switch(struct process *process) {
    input_dest_process = process;
    return 0;
}

struct process *process_get(int id) {
    if (id < 0 || id >= MAX_PROCESSES) {
        return NULL;
    }

    return processes[id];
}

int process_switch(struct process *process) {
    current_process = process;
    return 0;
}

void process_get_arguments(struct process *process, int *argc, char ***argv) {
    *argc = process->arguments.argc;
    *argv = process->arguments.argv;
}

int process_count_command_arguments(struct command_argument *root_argument) {
    struct command_argument *current = root_argument;
    int i = 0;
    while (current) {
        i++;
        current = current->next;
    }

    return i;
}

int process_inject_arguments(struct process *process, struct command_argument *root_argument) {
    int res = 0;
    struct command_argument *current = root_argument;
    int i = 0;
    int argc = process_count_command_arguments(root_argument);
    if (argc == 0) {
        res = -ERROR_IO;
        goto out;
    }

    char **argv = process_malloc(process, (sizeof(const char *) * argc));
    if (!argv) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    while(current) {
        char *argument_str = process_malloc(process, sizeof(current->argument));
        if (!argument_str) {
            res = -ERROR_NO_MEM;
            goto out;
        }

        strncpy(argument_str, current->argument, sizeof(current->argument));
        argv[i] = argument_str;
        current = current->next;
        i++;
    }

    process->arguments.argc = argc;
    process->arguments.argv = argv;

out:
    return res;
}

static int process_find_free_allocation_index(struct process *process) {
    int res = -ERROR_NO_MEM;
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++) {
        if (process->allocations[i].ptr == 0) {
            res = i;
            break;
        }
    }

    return res;
}

void *process_malloc(struct process *process, size_t size) {
    void *ptr = kzalloc(size);
    if (!ptr) {
        goto out_err;
    }

    int index = process_find_free_allocation_index(process);
    if (index < 0) {
        goto out_err;
    }
    
    int res = paging_map_to(process->task->page_directory, ptr, ptr, paging_align_address(ptr + size), PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (res < 0) {
        goto out_err;
    }

    process->allocations[index].ptr = ptr;
    process->allocations[index].size = size;
    return ptr;

out_err:
    if (ptr) {
        kfree(ptr);
    }

    return 0;
}

static void process_allocation_unjoin(struct process *process, void *ptr) {
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++) {
        if (process->allocations[i].ptr == ptr) {
            process->allocations[i].ptr = 0x00;
            process->allocations[i].size = 0;
        }
    }
}

static struct process_allocation *process_get_allocation_by_addr(struct process *process, void *addr) {
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++) {
        if (process->allocations[i].ptr == addr) {
            return &process->allocations[i];
        }
    }
}

void process_free(struct process *process, void *ptr) {    
    struct process_allocation *allocation = process_get_allocation_by_addr(process, ptr);
    if (!allocation) {
        return;
    }

    int res = paging_map_to(process->task->page_directory, allocation->ptr, allocation->ptr, paging_align_address(allocation->ptr + allocation->size), 0x00);
    if (res < 0) {
        return;
    }

    process_allocation_unjoin(process, ptr);
    kfree(ptr);
}

static int process_load_binary(const char *filename, struct process *process) {
    void *program_data_ptr = 0x00;

    int res = 0;
    int fd = fopen(filename, "r");
    if (!fd) {
        res = -ERROR_IO;
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res != ALL_OK) {
        goto out;
    }

    program_data_ptr = kzalloc(stat.filesize);
    if (!program_data_ptr) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1) {
        res = -ERROR_IO;
        goto out;
    }

    process->filetype = PROCESS_FILETYPE_BINARY;
    process->ptr = program_data_ptr;
    process->size = stat.filesize;

out:
    if (res < 0) {
        if (program_data_ptr) {
            kfree(program_data_ptr);
        }
    }

    fclose(fd);
    return res;
}

static int process_load_elf(const char *filename, struct process *process) {
    int res = 0;
    struct elf_file *elf_file = 0;
    res = elf_load(filename, &elf_file);
    if (res < 0) {
        goto out;
    }

    process->filetype = PROCESS_FILETYPE_ELF;
    process->elf_file = elf_file;

out:
    return res;
}

static int process_load_data(const char *filename, struct process *process) {
    int res = 0;
    res = process_load_elf(filename, process);
    if (res == -ERROR_INVALID_FORMAT) {
        res = process_load_binary(filename, process);
    }
    
    return res;
}

static int process_map_elf(struct process *process) {
    int res = 0;

    struct elf_file *elf_file = process->elf_file;
    struct elf_header *header = elf_header(elf_file);
    struct elf32_phdr *phdrs = elf_pheader(header);
    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr *phdr = &phdrs[i];
        void *phdr_phys_address = elf_phdr_physical_address(elf_file, phdr);
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (phdr->p_flags & PF_W) {
            flags |= PAGING_IS_WRITEABLE;
        }
    
        res = paging_map_to(process->task->page_directory, paging_align_to_lower_page((void *)phdr->p_vaddr), paging_align_to_lower_page(phdr_phys_address), paging_align_address(phdr_phys_address + phdr->p_memsz), flags);
        if (ISERR(res)) {
            break;
        }
    }

    return res;
}

int process_map_binary(struct process *process) {
    int res = 0;
    paging_map_to(process->task->page_directory, (void *)PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

int process_map_memory(struct process *process) {
    int res = 0;

    switch(process->filetype) {
        case PROCESS_FILETYPE_ELF:
            res = process_map_elf(process);
        break;

        case PROCESS_FILETYPE_BINARY:
            res = process_map_binary(process);
        break;

        default:
            panic("process_map_memory: invalid filetype\n", -ERROR_INVALID_FORMAT);
    }

    if (res < 0) {
        goto out;
    }

    paging_map_to(process->task->page_directory, (void *)PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack + USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
out:
    return res;
}

int process_get_free_slot() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i] == 0) {
            return i;
        }
    }

    return -ERROR_IS_TAKEN;
}

int process_load_for_slot(const char *filename, struct process **process, int process_slot) {
    int res = 0;
    struct process *_process;
    void *program_stack_ptr = 0;

    if (process_get(process_slot) != 0) {
        res = -ERROR_IS_TAKEN;
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0) {
        goto out;
    }

    _process->stack = kzalloc(USER_PROGRAM_STACK_SIZE);
    if (!_process->stack) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->id = process_slot;

    _process->task = task_new(_process);
    if (ISERR(_process->task)) {
        res = ERROR_I(_process->task);
        _process->task = NULL;
        goto out;
    }

    res = process_map_memory(_process);
    if (res < 0) {
        goto out;
    }

    *process = _process;
    processes[process_slot] = _process;

out:
    if (ISERR(res)) {
        if (_process) {
            process_memory_free(_process);
            _process = NULL;
            *process = NULL;
        }
    }

    return res;
}

int process_load(const char *filename, struct process **process) {
    int res = 0;
    int process_slot = process_get_free_slot();
    if (process_slot < 0) {
        res = -ERROR_IS_TAKEN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);

out:
    return res;
}

int process_load_switch(const char *filename, struct process **process) {
    int res = process_load(filename, process);
    if (res == ALL_OK) {
        process_switch(*process);
    }

    return res;
}