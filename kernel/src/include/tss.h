#pragma once

#include <stdint.h>

#include <process.h>
namespace TSS
{
    void InitializeTSS(tss_t* tss, void* gdt);

    inline void SetKernelStack(tss_t* tss, uint64_t stack){
        tss->rsp0 = stack;
    }
}