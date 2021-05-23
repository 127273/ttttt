#ifndef __ISR_H
#define __ISR_H

#include <idt.h>
#include <stdint.h>
#include <pic.h>




// exceptions, cf. http://wiki.osdev.org/Exceptions
#define EXCEPTION_DE 0
#define EXCEPTION_DB 1
#define EXCEPTION_BP 3
#define EXCEPTION_OF 4
#define EXCEPTION_BR 5
#define EXCEPTION_UD 6
#define EXCEPTION_NM 7
#define EXCEPTION_DF 8
#define EXCEPTION_TS 10
#define EXCEPTION_NP 11
#define EXCEPTION_SS 12
#define EXCEPTION_GP 13
#define EXCEPTION_PF 14

// IRQ 0..7 remaped to 0x20..0x27 
// IRQ 8..15 remaped to 0x28..0x37
#define ISR_IRQ0 (IRQ_BASE + PIC_IRQ0)
#define ISR_IRQ1 (IRQ_BASE + PIC_IRQ1)
#define ISR_IRQ2 (IRQ_BASE + PIC_IRQ2)
#define ISR_IRQ3 (IRQ_BASE + PIC_IRQ3)
#define ISR_IRQ4 (IRQ_BASE + PIC_IRQ4)
#define ISR_IRQ5 (IRQ_BASE + PIC_IRQ5)
#define ISR_IRQ6 (IRQ_BASE + PIC_IRQ6)
#define ISR_IRQ7 (IRQ_BASE + PIC_IRQ7)
#define ISR_IRQ8 (IRQ_BASE + PIC_IRQ8)
#define ISR_IRQ9 (IRQ_BASE + PIC_IRQ9)
#define ISR_IRQA (IRQ_BASE + PIC_IRQA)
#define ISR_IRQB (IRQ_BASE + PIC_IRQB)
#define ISR_IRQC (IRQ_BASE + PIC_IRQC)
#define ISR_IRQD (IRQ_BASE + PIC_IRQD)
#define ISR_IRQE (IRQ_BASE + PIC_IRQE)
#define ISR_IRQF (IRQ_BASE + PIC_IRQF)

#define SYSCALL_NO 0x80

// These functions are declared in interrupt.asm file
extern "C" void isr_stub_0();
extern "C" void isr_stub_1();
extern "C" void isr_stub_2();
extern "C" void isr_stub_3();
extern "C" void isr_stub_4();
extern "C" void isr_stub_5();
extern "C" void isr_stub_6();
extern "C" void isr_stub_7();
extern "C" void isr_stub_8();
extern "C" void isr_stub_9();
extern "C" void isr_stub_10();
extern "C" void isr_stub_11();
extern "C" void isr_stub_12();
extern "C" void isr_stub_13();
extern "C" void isr_stub_14();
extern "C" void isr_stub_15();
extern "C" void isr_stub_16();
extern "C" void isr_stub_17();
extern "C" void isr_stub_18();
extern "C" void isr_stub_19();
extern "C" void isr_stub_20();
extern "C" void isr_stub_21();
extern "C" void isr_stub_22();
extern "C" void isr_stub_23();
extern "C" void isr_stub_24();
extern "C" void isr_stub_25();
extern "C" void isr_stub_26();
extern "C" void isr_stub_27();
extern "C" void isr_stub_28();
extern "C" void isr_stub_29();
extern "C" void isr_stub_30();
extern "C" void isr_stub_31();
// IRQs
extern "C" void isr_stub_32();
extern "C" void isr_stub_33();
extern "C" void isr_stub_34();
extern "C" void isr_stub_35();
extern "C" void isr_stub_36();
extern "C" void isr_stub_37();
extern "C" void isr_stub_38();
extern "C" void isr_stub_39();
extern "C" void isr_stub_40();
extern "C" void isr_stub_41();
extern "C" void isr_stub_42();
extern "C" void isr_stub_43();
extern "C" void isr_stub_44();
extern "C" void isr_stub_45();
extern "C" void isr_stub_46();
extern "C" void isr_stub_47();
// SYSCALL
extern "C" void isr_stub_128();

struct isr_ctx_struct {
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;

	// Contains error code and interrupt number for exceptions
	// Contains syscall number for syscalls
	// Contains just the interrupt number otherwise
	uint32_t error_code;
	uint32_t int_no;
	// Interrupt stack frame
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};
typedef struct isr_ctx_struct isr_ctx_t;


typedef void (*isr_t)(isr_ctx_t *regs);

void init_kernel_isr();
void register_interrupt_handler(uint64_t id, isr_t handler);

namespace myos
{
	namespace hardware
	{
		class InterruptManager;

        class InterruptHandler
        {
        protected:
            uint8_t InterruptNumber;
            InterruptManager* interruptManager;
            InterruptHandler(InterruptManager* interruptManager, uint8_t InterruptNumber);
            ~InterruptHandler();
        public:
            virtual void HandleInterrupt(isr_ctx_t *regs);
        };

		class InterruptManager
		{
			friend class InterruptHandler;
            protected:
				
                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256];
               // TaskManager *taskManager;
			
			public:
				static void handleInterrupt(isr_ctx_t *regs);
				void enable();
				void disable();
				
				InterruptManager();
				void DoHandleInterrupt(isr_ctx_t *regs);
		};
	}
}


#endif
