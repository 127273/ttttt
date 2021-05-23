#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>

enum {
    KPRN_INFO,   
    KPRN_WARN,   
    KPRN_ERR,    
    KPRN_DBG,   
    KPRN_PANIC,  
};

enum{
    DebugLevelQuiet = 0,
    DebugLevelNormal = 1,
    DebugLevelVerbose = 2,
};

#define KERNEL_START_MEMORY 0xFFFF800000000000
#define KERNEL_VMA ((char*)KERNEL_START_MEMORY)

#define KERNEL_USER_VIEW_START_MEMORY 0xFFFFE00000000000
#define KERNEL_USER_VIEW_VMA ((char*)KERNEL_USER_VIEW_START_MEMORY)

#define KERNEL_VIDEO_MEMORY ((unsigned char*)0xFFFFC00000000000)

#define TO_VMA_U64(ptr) ((uint64_t)ptr + (uint64_t)KERNEL_START_MEMORY)
#define TO_VMA_PTR(type, ptr) ((type)TO_VMA_U64(ptr))
#define TO_PHYS_U64(ptr) ((uint64_t)ptr ^ (uint64_t)KERNEL_START_MEMORY)
#define TO_PHYS_PTR(type, ptr) ((type)TO_PHYS_U64(ptr))

#define GDT_NULL        0x00
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28 

void HALT_AND_CATCH_FIRE(const char *fmt, ...);
void _kdebug(const char *fmt, ...);
void color_kdebug(int colorFont, const char *fmt, ...) ;
#define __UNUSED__ __attribute__((unused))

#define DEBUG _kdebug

static	uintptr_t GetIOMapping(uintptr_t addr)
{
    if(addr > 0xffffffff){ // Typically most MMIO will not reside > 4GB, but check just in case
    DEBUG("MMIO >4GB current unsupported");
        return 0xffffffff;
    }
    return  TO_VMA_U64(addr);
}

static void wait_ms(int i)
{
    volatile int j = 0;
    volatile int k = (1 << 13) * i;
    while (j++ < k)
    {
        /* code */
    }
}
#endif
