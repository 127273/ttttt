#include <smp.h>

#include <apic.h>
#include <kernel.h>
#include <apic.h>
#include <sse.h>
#include <64bit.h>



#include <cpu.h>
//#include <device.h>
#include <apic.h>
//#include <logging.h>
#include <timer.h>
#include <acpi.h>
#include <tss.h>
#include <idt.h>
//#include <hal.h>
#include <memory.h>
#include <string.h>
#include <kernel.h>
#include <madt.h>
#include "smpdefines.inc"


// extern void* _binary_smptrampoline_bin_start;
// extern void* _binary_smptrampoline_bin_size;

volatile uint16_t* smpMagic = (uint16_t*)SMP_TRAMPOLINE_DATA_MAGIC;
volatile uint16_t* smpID = (uint16_t*)SMP_TRAMPOLINE_CPU_ID;
gdt_ptr_t* smpGDT = (gdt_ptr_t*)SMP_TRAMPOLINE_GDT_PTR;
volatile uint64_t* smpCR3 = (uint64_t*)SMP_TRAMPOLINE_CR3;
volatile uint64_t* smpStack = (uint64_t*)SMP_TRAMPOLINE_STACK;
volatile uint64_t* smpEntry2 = (uint64_t*)SMP_TRAMPOLINE_ENTRY2;

volatile bool doneInit = false;
volatile bool SMPloaded = false;
extern gdt_ptr_t GDT64Pointer64;

#define POKE(addr) (*((volatile uintptr_t *)(addr)))
extern "C" uint32_t trampoline_start, trampoline_end, nstack;
extern "C" void cpuupstart();
namespace SMP{
    CPU* cpus[256];
    unsigned processorCount = 1;
    tss_t tss1 __attribute__((aligned(16)));

    void SMPEntry(uint16_t id){
        CPU* cpu = cpus[id];
       // cpu->currentThread = nullptr;
        cpu->runQueueLock = 0;
        SetCPULocal(cpu);
        cpu->gdt = malloc(4096); 
       // cpu->gdt = Memory::KernelAllocate4KPages(1);//kmalloc(GDT64Pointer64.limit + 1); // Account for the 1 subtracted from limit
        //Memory::KernelMapVirtualMemory4K(Memory::AllocatePhysicalMemoryBlock(), (uintptr_t)cpu->gdt, 1);
        memcpy(cpu->gdt, (void*)GDT64Pointer64.base, GDT64Pointer64.limit + 1); // Make a copy of the GDT
        cpu->gdtPtr = {.limit = GDT64Pointer64.limit, .base = (uint64_t)cpu->gdt};

        asm volatile("lgdt (%%rax)" :: "a"(&cpu->gdtPtr));

      // idt_flush();

        TSS::InitializeTSS(&cpu->tss, cpu->gdt);

        APIC::Local::Enable();

       // cpu->runQueue = new FastList<Thread*>();
        
        doneInit = true;

        asm("sti");

        for(;;);
    }

    void PrepareTrampoline(){
    //    memcpy((void*)SMP_TRAMPOLINE_ENTRY, &_binary_smptrampoline_bin_start, ((uint64_t)&_binary_smptrampoline_bin_size));
    // memcpy((void*)SMP_TRAMPOLINE_ENTRY, &_binary_smptrampoline_bin_start, ((uint64_t)&_binary_smptrampoline_bin_size));
    }

    void InitializeCPU(uint16_t id){
        CPU* cpu = new CPU;
        cpu->id = id;
        cpu->runQueueLock = 0;
        cpus[id] = cpu;
        

        	unsigned int a,b,c,d;

	//check APIC & x2APIC support
	get_cpuid(1,0,&a,&b,&c,&d);
	//void get_cpuid(unsigned int Mop,unsigned int Sop,unsigned int * a,unsigned int * b,unsigned int * c,unsigned int * d)


	if((1<<9) & d)
		DEBUG("HW support APIC&xAPIC\t");
	else
		DEBUG("HW NO support APIC&xAPIC\t");
	
	if((1<<21) & c)
		DEBUG("HW support x2APIC\n");
	else
		DEBUG("HW NO support x2APIC\n");



        *smpMagic = 0; // Set magic to 0
        *smpID = id; // Set ID to our CPU's ID
        *smpEntry2 = (uint64_t)SMPEntry; // Our second entry point
       // *smpStack = (uint64_t)Memory::KernelAllocate4KPages(1); // 4K stack
       // Memory::KernelMapVirtualMemory4K(Memory::AllocatePhysicalMemoryBlock(), *smpStack, 1);
        *smpStack += PAGE_SIZE_4K;
        *smpGDT = GDT64Pointer64;

        asm volatile("mov %%cr3, %%rax" : "=a"(*smpCR3));

        APIC::Local::SendIPI(id, ICR_DSH_DEST, ICR_MESSAGE_TYPE_INIT, 0);

      //  wait(20);

        if((*smpMagic) !=  SMP_MAGIC){ // Check if the trampoline code set the flag to let us know it has started
            APIC::Local::SendIPI(id, ICR_DSH_DEST, ICR_MESSAGE_TYPE_STARTUP, (SMP_TRAMPOLINE_ENTRY >> 12));

          //  wait(80);
        }

        if((*smpMagic) != SMP_MAGIC){
         //   wait(100);
        }

        if((*smpMagic) != SMP_MAGIC){
           DEBUG("[SMP] Failed to start CPU #%d", id);
            return;
        }
        
        while(!doneInit);

        doneInit = false;
    }

    void Initialize(){
        cpus[0] = new CPU; // Initialize CPU 0
        cpus[0]->id = 0;
        cpus[0]->gdt = (void*)GDT64Pointer64.base;
        cpus[0]->gdtPtr = GDT64Pointer64;
        //cpus[0]->currentThread = nullptr;
        cpus[0]->runQueueLock = 0;
        //cpus[0]->runQueue = new FastList<Thread*>();
        SetCPULocal(cpus[0]);

        // if(HAL::disableSMP) {
        //     TSS::InitializeTSS(&cpus[0]->tss, cpus[0]->gdt);
        //     ACPI::processorCount = 1;
        //     processorCount = 1;
        //     return;
        // }

        processorCount = ACPI::processorCount;

        PrepareTrampoline();

         InitializeCPU(1);
        // for(int i = 0; i < processorCount; i++){
        //     if(ACPI::processors[i] != 0)
        //         InitializeCPU(ACPI::processors[i]);
        // }
        
        TSS::InitializeTSS(&cpus[0]->tss, cpus[0]->gdt);

        DEBUG("[SMP] %u processors initialized!", processorCount);
    }
}

extern "C" void cpuupstart(void)
{
  DEBUG("slave cpu start\n");
  SMPloaded = true;
  while(1);
}


#define current_cpu ({ \
    size_t cpu_number; \
    asm volatile ("mov %0, qword ptr gs:[0]" \
                    : "=r" (cpu_number) \
                    : \
                    : "memory", "cc"); \
    (int)cpu_number; \
})

// static void ap_kernel_entry(void) {
//     /* APs jump here after initialisation */

//     color_kdebug(KPRN_INFO, "smp: Started up AP #%u", current_cpu);

//     /* Enable this AP's local APIC */
//   //  init_cpu_features();
//    // lapic_enable();

//     /* Enable interrupts */
//     asm volatile ("sti");

//     /* Wait for some job to be scheduled */
//     for (;;) asm volatile ("hlt");
// }

// static inline void setup_cpu_local(int cpu_number, uint8_t lapic_id) {
//     /* Set up stack guard page */
//     unmap_page(kernel_pagemap, (size_t)&cpu_stacks[cpu_number].guard_page[0]);

//     /* Prepare CPU local */
//     cpu_locals[cpu_number].cpu_number = cpu_number;
//     cpu_locals[cpu_number].kernel_stack = (size_t)&cpu_stacks[cpu_number].stack[CPU_STACK_SIZE];
//     cpu_locals[cpu_number].current_process = -1;
//     cpu_locals[cpu_number].current_thread = -1;
//     cpu_locals[cpu_number].current_task = -1;
//     cpu_locals[cpu_number].lapic_id = lapic_id;

//     /* Prepare TSS */
//     cpu_tss[cpu_number].rsp0 = (uint64_t)&cpu_stacks[cpu_number].stack[CPU_STACK_SIZE];
//     cpu_tss[cpu_number].ist1 = (uint64_t)&cpu_stacks[cpu_number].stack[CPU_STACK_SIZE];
// }

// static int start_ap(uint8_t target_apic_id, int cpu_number) {
//     if (cpu_number == MAX_CPUS) {
//         panic(NULL, 0, "smp: CPU limit exceeded");
//     }

//     setup_cpu_local(cpu_number, target_apic_id);

//     struct cpu_local_t *cpu_local = &cpu_locals[cpu_number];
//     struct tss_t *tss = &cpu_tss[cpu_number];
//     uint8_t *stack = &cpu_stacks[cpu_number].stack[CPU_STACK_SIZE];

//     void *trampoline = smp_prepare_trampoline(ap_kernel_entry, (void *)kernel_pagemap->pml4,
//                                               stack, cpu_local, tss);

//     /* Send the INIT IPI */
//     lapic_write(APICREG_ICR1, ((uint32_t)target_apic_id) << 24);
//     lapic_write(APICREG_ICR0, 0x500);
//     /* wait 10ms */
//     ksleep(10);
//     /* Send the Startup IPI */
//     lapic_write(APICREG_ICR1, ((uint32_t)target_apic_id) << 24);
//     lapic_write(APICREG_ICR0, 0x600 | (uint32_t)(size_t)trampoline);

//     for (int i = 0; i < 1000; i++) {
//         ksleep(1);
//         if (smp_check_ap_flag())
//             return 0;
//     }
//     return -1;
// }

// static void init_cpu0(void) {
//     setup_cpu_local(0, 0);

//     struct cpu_local_t *cpu_local = &cpu_locals[0];
//     struct tss_t *tss = &cpu_tss[0];

//     smp_init_cpu0_local(cpu_local, tss);
// }

// void init_smp(void) {
//     /* prepare CPU 0 first */
//     init_cpu0();

//     /* start up the APs and jump them into the kernel */
//     for (size_t i = 1; i < madt_local_apic_i; i++) {
//         /* Check if LAPIC is marked as disabled */
//         uint32_t flags = madt_local_apics[i]->flags;
//         if (!((flags & 1) ^ ((flags >> 1) & 1))) {
//             kprint(KPRN_INFO, "smp: Theoretical AP #%u ignored", i);
//             continue;
//         }

//         kprint(KPRN_INFO, "smp: Starting up AP #%u", i);

//         if (start_ap(madt_local_apics[i]->apic_id, smp_cpu_count)) {
//             kprint(KPRN_ERR, "smp: Failed to start AP #%u", i);
//             continue;
//         }

//         smp_cpu_count++;
//         /* wait a bit */
//         ksleep(10);
//     }

//     col(KPRN_INFO, "smp: Total CPU count: %u", smp_cpu_count);

//     smp_ready = 1;
// }
