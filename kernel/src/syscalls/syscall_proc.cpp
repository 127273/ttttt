#include <window_manager.h>
#include <mmu_heap.h>
#include <process.h>
#include <kernel.h>
#include <string.h>
#include <vesa.h>
#include <gfx.h>
#include <isr.h>

// syscall_process_create(void* elf_data)
uint64_t syscall_process_create(isr_ctx_t *regs) {
	void *elf_data = (void*)regs->rdi;
	task_t *task = create_user_process(elf_data);
	
	return task != NULL ? task->id : 0;
}

// syscall_process_create(void* elf_data)
void syscall_process_create2(isr_ctx_t *regs) {
	void *elf_data = (void*)(regs->rdi);
	task_t *task = create_user_process(elf_data);
	
	uint64_t *result = (uint64_t*)(regs->rsi);
	*result = ((task != NULL) ? task->id : 0);
}


// syscall_process_create(void* elf_data)
uint64_t syscall_process_from_file(isr_ctx_t *regs) {
	char *filename = (char*)regs->rdi;
	task_t *task = create_user_process_file(filename);
	return task != NULL ? task->id : 0;
}

void syscall_process_from_file2(isr_ctx_t *regs) {
	char *filename = (char*)regs->rdi;
	task_t *task = create_user_process_file(filename);
	uint64_t *result = (uint64_t*)(regs->rsi);
	*result = ((task != NULL) ? task->id : 0);
}

// syscall_process_exit(int exit_code)
uint64_t syscall_process_exit(isr_ctx_t *regs __UNUSED__) {
	// int exit_code = regs->rdi;
	return kill_process_id(task_list_current->id);
}

void syscall_process_exit2(isr_ctx_t *regs __UNUSED__) {
	// int exit_code = regs->rdi;
	uint64_t *result = (uint64_t*)(regs->rdi);
	*result = kill_process_id(task_list_current->id);
}

uint64_t syscall_process_get_id(isr_ctx_t *regs __UNUSED__) {
	return task_list_current->id;
}

void syscall_process_get_id2(isr_ctx_t *regs __UNUSED__) {
	uint64_t *result = (uint64_t*)(regs->rdi);
	*result = task_list_current->id;
}

void syscall_open(isr_ctx_t *regs __UNUSED__) {
	DEBUG("syscall_open \n\n");
	return ;
}
// syscall_process_clone(void* entry_point, void* task_stack)
uint64_t syscall_process_clone(isr_ctx_t *regs __UNUSED__) {
	uint64_t entry_point = regs->rdi;
	uint64_t stack_address = regs->rsi;
	task_t *task = create_user_process_clone(task_list_current, entry_point, stack_address);
	return task != NULL ? task->id : 0;
}

void syscall_process_clone2(isr_ctx_t *regs __UNUSED__) {
	uint64_t entry_point = regs->rdi;
	uint64_t stack_address = regs->rsi;
	task_t 	*task = create_user_process_clone(task_list_current, entry_point, stack_address);
	uint64_t *result = (uint64_t*)(regs->rdx);

	*result = ((task != NULL) ? task->id : 0);
}
