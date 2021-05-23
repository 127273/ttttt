#ifndef __SYCALL_H
#define __SYCALL_H


enum {
  SYSCall_windows_create = 0,
  SYSCall_memory_sbrk,
  SYSCall_debug_puts,      	
  SYSCall_messaging_get, 		
  SYSCall_messaging_create, 	
  SYSCall_windows_present,  	
  SYSCall_process_create,		
  SYSCall_file_open,			
  SYSCall_file_read,			
  SYSCall_process_from_file,  
  SYSCall_messaging_peek,     
  SYSCall_process_exit,       
  SYSCall_memory_stats,       
  SYSCall_dir_open,           
  SYSCall_dir_read_next,      
  SYSCall_file_write,         
  SYSCall_process_clone,	   
  SYSCall_process_get_id,     
  SYSCall_get_VGA_information,
  SYSCall_syscall_open,
  SYSCall_Get_Time,
  SYSCall_execve,
  SYSCall_fork,
  SYSCall_fstat,
  SYSCall_getpid,
  SYSCall_isatty,
  SYSCall_kill,
  SYSCall_link,
  SYSCall_lseek,
  SYSCall_stat,
  SYSCall_unlink,
  SYSCall_wait,
  SYSCall_mem,

  SYSCall_Tables_Length
};



#define __SYSCALL_VOID(nr, sym) extern void sym(isr_ctx_t *regs);
#define __SYSCALL(nr, sym) extern uint64_t sym(isr_ctx_t *regs);

  __SYSCALL_VOID(SYSCall_windows_create,   syscall_windows_create)
__SYSCALL(SYSCall_memory_sbrk,      syscall_memory_sbrk)
__SYSCALL_VOID(SYSCall_debug_puts,       syscall_debug_puts)
__SYSCALL_VOID(SYSCall_messaging_get,    syscall_messaging_get)
__SYSCALL_VOID(SYSCall_messaging_create, syscall_messaging_create)
__SYSCALL_VOID(SYSCall_windows_present,  syscall_windows_present)
__SYSCALL(SYSCall_process_create,   syscall_process_create)
__SYSCALL_VOID(SYSCall_file_open,        syscall_file_open)
__SYSCALL_VOID(SYSCall_file_read,        syscall_file_read)
__SYSCALL(SYSCall_process_from_file,syscall_process_from_file)
__SYSCALL_VOID(SYSCall_messaging_peek,   syscall_messaging_peek)
__SYSCALL(SYSCall_process_exit,     syscall_process_exit)
__SYSCALL_VOID(SYSCall_memory_stats,     syscall_memory_stats)
__SYSCALL_VOID(SYSCall_dir_open,         syscall_dir_open)
__SYSCALL_VOID(SYSCall_dir_read_next,    syscall_dir_read_next)
__SYSCALL(SYSCall_file_write,       syscall_file_write)
__SYSCALL(SYSCall_process_clone,    syscall_process_clone)
__SYSCALL(SYSCall_process_get_id,   syscall_process_get_id)

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

#undef __SYSCALL
#undef __SYSCALL_VOID

#endif

