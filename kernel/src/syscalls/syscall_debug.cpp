#include <process.h>
#include <kernel.h>
#include <string.h>
#include <isr.h>
#include <x86.h>

#define __UNUSED__ __attribute__((unused))

// syscall_debug_puts(char* string)
void syscall_debug_puts(isr_ctx_t *regs) {
	DEBUG("APP[%i]: %s", task_list_current->id, regs->rdi);
	
}

void syscall_get_Time(isr_ctx_t *regs)
{
	//year month day hout min second
	regs->rdi;
	regs->rsi;
	regs->rdx;
	regs->rcx;
	regs->r8;

}
