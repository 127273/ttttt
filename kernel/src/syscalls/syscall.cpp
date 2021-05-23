#include <process.h>
#include <kernel.h>
#include <stdint.h>
#include <isr.h>


typedef void (*sys_call_ptr_void)(isr_ctx_t *regs);




#include <syscalls.h>



extern "C"{
#define __SYSCALL(nr, sym) [nr] = sym,
#define __SYSCALL_VOID(nr, syms) [nr] = syms,

const sys_call_ptr_void sys_call_table[] = {
 __SYSCALL_VOID(SYSCall_windows_create,   syscall_windows_create)
__SYSCALL(SYSCall_memory_sbrk,      (sys_call_ptr_void)syscall_memory_sbrk)
__SYSCALL_VOID(SYSCall_debug_puts,       syscall_debug_puts)
__SYSCALL_VOID(SYSCall_messaging_get,    syscall_messaging_get)
__SYSCALL_VOID(SYSCall_messaging_create, syscall_messaging_create)
__SYSCALL_VOID(SYSCall_windows_present,  syscall_windows_present)
__SYSCALL(SYSCall_process_create,   (sys_call_ptr_void)syscall_process_create)
__SYSCALL_VOID(SYSCall_file_open,        syscall_file_open)
__SYSCALL_VOID(SYSCall_file_read,        syscall_file_read)
__SYSCALL(SYSCall_process_from_file,(sys_call_ptr_void)syscall_process_from_file)
__SYSCALL_VOID(SYSCall_messaging_peek,   syscall_messaging_peek)
__SYSCALL(SYSCall_process_exit,     (sys_call_ptr_void)syscall_process_exit)
__SYSCALL_VOID(SYSCall_memory_stats,     syscall_memory_stats)
__SYSCALL_VOID(SYSCall_dir_open,         syscall_dir_open)
__SYSCALL_VOID(SYSCall_dir_read_next,    syscall_dir_read_next)
__SYSCALL(SYSCall_file_write,       (sys_call_ptr_void)syscall_file_write)
__SYSCALL(SYSCall_process_clone,    (sys_call_ptr_void)syscall_process_clone)
__SYSCALL(SYSCall_process_get_id,   (sys_call_ptr_void)syscall_process_get_id)

__SYSCALL_VOID(SYSCall_get_VGA_information, syscall_get_VGA_information)


__SYSCALL_VOID(SYSCall_syscall_open,  syscall_open)
__SYSCALL_VOID(SYSCall_Get_Time,      syscall_get_Time)

__SYSCALL_VOID(SYSCall_execve,  syscall_execve)
__SYSCALL_VOID(SYSCall_fork,    syscall_fork)
__SYSCALL_VOID(SYSCall_fstat,   syscall_fstat)
__SYSCALL_VOID(SYSCall_getpid,  syscall_getpid)
__SYSCALL_VOID(SYSCall_isatty,  syscall_isatty)
__SYSCALL_VOID(SYSCall_kill,    syscall_kill)
__SYSCALL_VOID(SYSCall_link,    syscall_link)
__SYSCALL_VOID(SYSCall_lseek,   syscall_lseek) 
__SYSCALL_VOID(SYSCall_stat,    syscall_stat)
__SYSCALL_VOID(SYSCall_unlink,  syscall_unlink)
__SYSCALL_VOID(SYSCall_wait,    syscall_wait)
__SYSCALL_VOID(SYSCall_mem,     syscall_mem)

};

}







//RDI, RSI, RDX, RCX, R8, R9
extern "C" uint64_t system_call_handler(isr_ctx_t *regs) {
  // DEBUG("SYSCALL[%i]:  %i(%i %i %i %i %i %i)\n", task_list_current->id, regs->rax, regs->rdi, 
  // 			regs->rsi, regs->rdx, regs->rcx, regs->r8, regs->r9);
  if(regs->rax != SYSCall_debug_puts)
    DEBUG("syscall vector: 0x%x\n", regs->rax);
  uint64_t vector = regs->rax;
  
  if (vector < SYSCall_Tables_Length) {
   (sys_call_table[vector])(regs);
   	return 0;
  }
  DEBUG("SYSCALL: %i not found! %i\n", regs->rax,sizeof(sys_call_table));
	return 0xFFFFFFFFFFFFFFFF;
}

void syscall_execve(isr_ctx_t *regs)
{

} 

void syscall_fstat(isr_ctx_t *regs)
{

}

void syscall_getpid(isr_ctx_t *regs)
{

}

void syscall_isatty(isr_ctx_t *regs)
{

}

void syscall_kill(isr_ctx_t *regs)
{

}

void syscall_link(isr_ctx_t *regs)
{

}

void syscall_lseek(isr_ctx_t *regs)
{

}

void syscall_stat(isr_ctx_t *regs)
{

}

void syscall_unlink(isr_ctx_t *regs)
{

}

void syscall_wait(isr_ctx_t *regs)
{

}

void syscall_fork(isr_ctx_t *regs)
{
  
}