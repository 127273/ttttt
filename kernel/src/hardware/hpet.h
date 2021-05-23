#pragma once

#include <acpi.h>
#include <driver.h>
#include <isr.h>

typedef struct address_structure
{
    uint8_t address_space_id;    // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;       //4 uint8 build COUNTER_CLK_PERIOD ???
    uint64_t address;   
} __attribute__((packed));

typedef struct HPET : public  acpi_header_t
{
    /** General Capabilities and ID Register*/
    uint8_t hardware_rev_id;
    uint8_t comparator_count:5;
    uint8_t counter_size:1;
    uint8_t reserved:1;
    uint8_t legacy_replacement:1;
    uint16_t pci_vendor_id;
    address_structure address;
    //uint32_t COUNTER_CLK_PERIOD;
    // uint64_t GEN_CONF;
    // uint64_t reserved1;
    // uint64_t GEN_Int_Status; COUNTER_CLK_PERIOD
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} __attribute__((packed));


extern HPET* hpet;
extern bool hpet_init();


namespace Aliceos{
     namespace drivers{
    
        class HEPTTimerControl : public myos::hardware::InterruptHandler, public myos::drivers::Driver
        {
        
            uint8_t interruptLine;
        public:
            HEPTTimerControl(myos::hardware::InterruptManager* manager, uint8_t interruptLine);
            ~HEPTTimerControl();
            void HandleInterrupt(isr_ctx_t *regs);
            void Activate();
        };
     }
}