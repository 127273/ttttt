#include <apic.h>

#include <kernel.h>
#include <cpu.h>
#include <debug.h>
#include <acpi.h>
#include <pic.h>
#include <system.h>
#include <isr.h>

#define LOCAL_APIC_ID 0x20 // APIC ID Register
#define LOCAL_APIC_VERSION 0x30 // APIC Version Register
#define LOCAL_APIC_TPR 0x80 // Task Priority Register 
#define LOCAL_APIC_APR 0x90 // Arbitration Priority Register
#define LOCAL_APIC_PPR 0xA0 // Processor Priority Register
#define LOCAL_APIC_EOI 0xB0 // End of Interrupt Register
#define LOCAL_APIC_RRD 0xC0 // Remote Read Register
#define LOCAL_APIC_LDR 0xD0 // Logical Destination Register
#define LOCAL_APIC_DFR 0xE0 // Destination Format Register
#define LOCAL_APIC_SIVR 0xF0 // Spurious Interrupt Vector Register
#define LOCAL_APIC_ISR 0x100 // In-service Register
#define LOCAL_APIC_TMR 0x180 // Trigger Mode Register
#define LOCAL_APIC_IRR 0x200 // Interrupt Request Register
#define LOCAL_APIC_ERROR_STATUS 0x280 // Error Status Register
#define LOCAL_APIC_ICR_LOW 0x300 // Interrupt Command Register Low
#define LOCAL_APIC_ICR_HIGH 0x310 // Interrupt Command Register High
#define LOCAL_APIC_LVT_TIMER 0x320 // Timer Local Vector Table Entry
#define LOCAL_APIC_LVT_THERMAL 0x330 // Thermal Local Vector Table Entry
#define LOCAL_APIC_LVT_PERF_MONITORING 0x340 // Performance Local Vector Table Entry
#define LOCAL_APIC_LVT_LINT0 0x350 // Local Interrupt 0 Local Vector Table Entry
#define LOCAL_APIC_LVT_LINT1 0x360 // Local Interrupt 1 Local Vector Table Entry
#define LOCAL_APIC_LVT_ERROR 0x370 // Error Local Vector Table Entry
#define LOCAL_APIC_TIMER_INITIAL_COUNT 0x380 // Timer Initial Count Register
#define LOCAL_APIC_TIMER_CURRENT_COUNT 0x390 // Timer Current Count Register
#define LOCAL_APIC_TIMER_DIVIDE 0x3E0 // Timer Divide Configuration Register

#define LOCAL_APIC_BASE 0xFFFFFFFFFF000

#define ICR_VECTOR(x) (x & 0xFF)
#define ICR_MESSAGE_TYPE_FIXED 0
#define ICR_MESSAGE_TYPE_LOW_PRIORITY (1 << 8)
#define ICR_MESSAGE_TYPE_SMI (2 << 8)
#define ICR_MESSAGE_TYPE_REMOTE_READ (3 << 8)
#define ICR_MESSAGE_TYPE_NMI (4 << 8)
#define ICR_MESSAGE_TYPE_INIT (5 << 8)
#define ICR_MESSAGE_TYPE_STARTUP (6 << 8)
#define ICR_MESSAGE_TYPE_EXTERNAL (7 << 8)

#define ICR_DSH_DEST 0          // Use destination field
#define ICR_DSH_SELF (1 << 18)  // Send to self
#define ICR_DSH_ALL (2 << 18)   // Send to ALL APICs
#define ICR_DSH_OTHER (3 << 18) // Send to all OTHER APICs 

#define IO_APIC_REGSEL 0x00 // I/O APIC Register Select Address Offset
#define IO_APIC_WIN 0x10 // I/O APIC I/O Window Address offset

#define IO_APIC_REGISTER_ID 0x0 // ID Register
#define IO_APIC_REGISTER_VER 0x1 // Version Register
#define IO_APIC_REGISTER_ARB 0x2 // I/O APIC Arbitration ID
#define IO_APIC_RED_TABLE_START 0x10 // I/O APIC Redirection Table Start
#define IO_APIC_RED_TABLE_ENT(x) (0x10 + 2 * x)

#define IO_RED_TBL_VECTOR(x) (x & 0xFF)

#define APIC_LVT_INT_MASKED (1 << 16)
#define APIC_LVT_TIMER_MODE_PERIODIC (1 << 17)



namespace APIC{
    namespace Local {
        uintptr_t base;
        volatile uintptr_t virtualBase;


        uint64_t ReadBase(){
            uint64_t low;
            uint64_t high;
            asm("rdmsr" : "=a"(low), "=d"(high) : "c"(0x1B));

            return (high << 32) | low;
        }

        void WriteBase(uint64_t val){
            uint64_t low = val & 0xFFFFFFFF;
            uint64_t high = val >> 32;
            asm("rdmsr" :: "a"(low), "d"(high), "c"(0x1B));
        }

        void Enable(){
            WriteBase(ReadBase() | (1UL << 11));
                                                            //0x1ff
            Write(LOCAL_APIC_SIVR, Read(LOCAL_APIC_SIVR) |  0x1ff  /* Enable APIC, Vector 255*/);
        }

        void SpuriousInterruptHandler(void*, RegisterContext* r){
            DEBUG("[APIC] Spurious Interrupt");
        }
        int Initialize(){
            Local::base = ReadBase() & LOCAL_APIC_BASE;
            virtualBase = TO_VMA_U64(base);

           
            DEBUG("[APIC] Local APIC Base %x (%x)\n", base, virtualBase);
        

            register_interrupt_handler(0xff, (isr_t)SpuriousInterruptHandler);
            Enable();


            return 0;
        }

        uint32_t Read(uint32_t off){
            return *((volatile uint32_t*)(virtualBase + off));
        }
        
        void Write(uint32_t off, uint32_t val){
            *((volatile uint32_t*)(virtualBase + off)) = val;
        }

        void SendIPI(uint8_t destination, uint32_t dsh /* Destination Shorthand*/, uint32_t type, uint8_t vector){
            uint32_t high = ((uint32_t)destination) << 24;
            uint32_t low = dsh | type | ICR_VECTOR(vector);

            Write(LOCAL_APIC_ICR_HIGH, high);
            Write(LOCAL_APIC_ICR_LOW, low);
        }

        void SendIPI(uint64_t value)
        {
            Write(LOCAL_APIC_ICR_HIGH, (value >> 32));
            Write(LOCAL_APIC_ICR_LOW, (value & 0xffffffff));
        }
    }

    namespace IO {
        uintptr_t base = 0;
        uintptr_t virtualBase;
        volatile uint32_t* registerSelect;
        volatile uint32_t* ioWindow;

        uint32_t interrupts;
        uint32_t apicID;

        uint32_t Read32(uint32_t reg){
            *registerSelect = (reg + virtualBase);
            return *ioWindow;
        }

        void Write32(uint32_t reg, uint32_t data){
           
            *registerSelect = (reg + virtualBase);
            *ioWindow = data;
    
        }
        
        uint64_t Read64(uint32_t reg){
      
            return 0;
        }

        void Write64(uint32_t reg, uint64_t data){
       
            uint32_t low = data & 0xFFFFFFFF;
            uint32_t high = data >> 32;

            Write32(reg, low);
            Write32(reg + 1, high);
        }

        void Redirect(uint8_t irq, uint8_t vector, uint32_t delivery){
            Write64(IO_APIC_RED_TABLE_ENT(irq), delivery | vector);
        }

        int Initialize(){
            if(!base){
                DEBUG("[APIC] Attempted to initialize I/O APIC without setting base");
                return 1;
            }

            virtualBase = TO_VMA_U64(base);
            
            registerSelect = (uint32_t*)(virtualBase + IO_APIC_REGSEL);
            ioWindow = (uint32_t*)(virtualBase + IO_APIC_WIN);

            interrupts = Read32(IO_APIC_REGISTER_VER) >> 16;
            apicID = Read32(IO_APIC_REGISTER_ID) >> 24;

            
            DEBUG("\n[APIC] I/O APIC Base %x (%x), Available Interrupts: %d, ID: %d ", base, virtualBase, interrupts, apicID);
          

        /** why this command page_fault ? just irq != vector ????
         *    Redirect(2, 0 + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
         */
             //Redirect(1, 1 + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
            Redirect(0, 0 + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
           
            Redirect(5, 5 + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
            Redirect(9, 9 + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
            Redirect(0xa, 0xa + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
            //Redirect(0xb, 0xb + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
            //Redirect(0xc, 0xc + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
           // Redirect(0xd, 0xd + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
            Redirect(0xe, 0xe + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
            Redirect(0xf, 0xf + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
        //    *registerSelect= (0xff80000);
        //    *ioWindow = 0x800;
         /*   for(unsigned i = 0; i < ACPI::isos->get_length(); i++){
                apic_iso_t* iso = ACPI::isos->get_at(i);

                if(debugLevelInterrupts >= DebugLevelVerbose){
				    DEBUG("[APIC] Interrupt Source Override, IRQ: %d, GSI: %d", iso->irqSource, iso->gSI);
                }
                Redirect(iso->gSI, iso->irqSource + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
               
            }
*/
            return 0;
        }

        void SetBase(uintptr_t newBase){
            base = newBase;
        }


        

        void MapLegacyIRQ(uint8_t irq){
            /*if(debugLevelInterrupts >= DebugLevelVerbose){
                Log::Info("[APIC] Mapping Legacy IRQ, IRQ: %d", irq);
            }
            for(unsigned i = 0; i < ACPI::isos->get_length(); i++){
                apic_iso_t* iso = ACPI::isos->get_at(i);
                if(iso->irqSource == irq) return; // We should have already redirected this IRQ
            }*/
            Redirect(irq, irq + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
        }
    }

    int Initialize(){
        cpuid_info_t cpuid = CPUID();

        if(!(cpuid.features_edx & CPUID_EDX_APIC)) {
            DEBUG("APIC Not Present");
            return 1;
        }

        asm("cli");

        init_kernel_pic();
        Local::Initialize();
        IO::Initialize();

        asm   volatile ("sti");
        DEBUG("ober");
        return 0;
    }

    
    void init_processor(uint32_t processorid, uint64_t entry)
    {
        
        Local::Write(icr2, (processorid << 24));
        Local::Write(icr1, 0x500);

        volatile int i = 1 << 25;
        while(i-- > 0);
        Local::Write(icr2, (processorid << 24));
        Local::Write(icr1, 0x600 | ((uint32_t)entry / 4096));

    }


}



    
extern "C" void LocalAPICEOI()
{
    // virtual 为0时中断就来了
//    DEBUG("EOI %x\t",  APIC::IO::virtualBase);
    APIC::Local::Write(LOCAL_APIC_EOI, 0);
  //  DEBUG("EOI ");
}

void send_ipi(uint8_t cpu, uint32_t interrupt_num)
{
    interrupt_num = (1 << 14) | interrupt_num;

    APIC::Local::Write(icr2, (cpu << 24));
    APIC::Local::Write(icr1, interrupt_num);
}