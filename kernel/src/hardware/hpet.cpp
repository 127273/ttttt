#include <hpet.h>
#include <apic.h>
#include <window_manager.h>
HPET* hpet = NULL;

bool hpet_init()
{
   
    //DEBUG("hpet number 0x%x\n", hpet->COUNTER_CLK_PERIOD);
    DEBUG("hpet vendor_id 0x%x\n", hpet->address.address_space_id | \
    hpet->address.register_bit_offset | hpet->address.register_bit_width | \
    hpet->address.reserved);
   
    //hpet->address.address |= 1 << 12;
    //hpet->minimum_tick = 1 << 14;
   // DEBUG("hpet conf 0x%x\n", hpet->GEN_CONF);
    DEBUG("%s\n", hpet->signature);
}

using namespace Aliceos;
using namespace drivers;
HEPTTimerControl::HEPTTimerControl(myos::hardware::InterruptManager* manager, uint8_t interruptLine)
: InterruptHandler(manager, interruptLine)
{
    this->interruptLine  = interruptLine;
}

HEPTTimerControl::~HEPTTimerControl()
{

}

void HEPTTimerControl::HandleInterrupt(isr_ctx_t *regs)
{
   // DEBUG("hpet inter ");
   window_manager_redraw();
}

void HEPTTimerControl::Activate()
{
    
    //hpet->GEN_CONF |= 0x3;
    uintptr_t base = (uintptr_t)&hpet->hardware_rev_id;
    mem_outl(base + 0x4, 0x05F5E100 / 1000);
    DEBUG("aa: %x,%x \t", mem_inl(base + 0x4),  hpet->address.address_space_id | \
    hpet->address.register_bit_offset | hpet->address.register_bit_width | \
    hpet->address.reserved);
    mem_outq(base + 0x100 , 0x004c);
    mem_outq(base + 0x108 , 0xDA7A63);
    mem_outq(base + 0xf0, 0);
     //DEBUG("hpet Activate %x \n",hpet->GEN_CONF);
     /** why this I sub 0x20 and in MaoLegacyIRQ function add 0x20 right now
      * I afraid this parameter - 0x20 < 0 
      */
     APIC::IO::MapLegacyIRQ(interruptLine - 0x20);
}