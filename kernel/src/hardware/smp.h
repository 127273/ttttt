#pragma once
#include <madt.h>
#include <cpu.h>
#include <tss.h>


namespace SMP{
    extern CPU* cpus[];
    extern unsigned processorCount;

    void InitializeCPU(uint16_t id);
    void Initialize();
}

