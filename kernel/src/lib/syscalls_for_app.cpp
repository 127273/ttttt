#include <isr.h>

#define __SYSCALL_VOID(nr, sym) extern void sym(isr_ctx_t *regs);
#define __SYSCALL(nr, sym) extern uint64_t sym(isr_ctx_t *regs);

#include <syscalls.h>
#include "syscalls_for_app.h"


int close(int file)
{
    //syscall(SYSCall_debug_puts, "This is close function \n");
    return 0;
}
//char **environ; /* pointer to array of char * strings that define the current environment variables */
int execve(char *name, char **argv, char **env)
{
    return 0;
}

int fork()
{
    return 0;
}

int fstat(int file, struct stat *st)
{
    return 0;
}

int getpid()
{
    return 0;
}

int isatty(int file)
{
    return 0;
}

int kill(int pid, int sig)
{
    return 0;
}

int link(char *old, char *news)
{
    return 0;
}

int lseek(int file, int ptr, int dir)
{
    return 0;
}

int open(const char *name, int flags, ...)
{
    //syscall(SYSCall_file_open);
    return 0;
}

int read(int file, char *ptr, int len)
{
    //syscall(SYSCall_file_read);
    return 0;
}
//caddr_t sbrk(int incr);
int stat(const char *file, struct stat *st)
{
    return 0;
}
//clock_t times(struct tms *buf);
int unlink(char *name)
{
    return 0;
}

int wait(int *status)
{
    return 0;
}

int write(int file, char *ptr, int len)
{
    return 0;
}

// int gettimeofday(struct timeval *p, struct timezone *z)
// {
//     return 0;
// }