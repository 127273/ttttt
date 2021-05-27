#include "net.h"

#include <pci.h>
#include <drivers/net/i8254/i8254.h>
#include <isr.h>
#include <mmu_heap.h>
using namespace myos::drivers;

NetDataHandlerBaseClass ::NetDataHandlerBaseClass()
{

}

NetDataHandlerBaseClass ::~NetDataHandlerBaseClass()
{
}

bool NetDataHandlerBaseClass::Receive()
{
    return false;
}

bool NetDataHandlerBaseClass::Send(uint8_t *buffer, uint32_t size)
{
    UNUSED(buffer);
    UNUSED(size);
    return false;
}

uintptr_t NetDataHandlerBaseClass::GetMACAddress()
{
    return 0;
}

uintptr_t NetDataHandlerBaseClass::GetIPAddress()
{
    return 0;
}

void NetDataHandlerBaseClass::SetIPAddress(uint32_t ip)
{
    
}

NetDataHandlerBaseClass *GetNetCard(myos::drivers::DriverManager *drvManager)
{
    NetDataHandlerBaseClass *net = NULL;
   // myos::hardware::InterruptManager *GlobalInterrupts;
    if (drvManager->driverGetBaseAddress(0x8086, 0x100f) == true)
    {
        net = (NetDataHandlerBaseClass*)malloc(sizeof(NetDataHandlerBaseClass));
        //new (net) intel_82545(drvManager->driverGetBaseAddress(0x8086, 0x100f), 0x10, GlobalInterrupts);
    }
      return net;
}

uint32_t NetDataHandlerBaseClass::GetSpeed()
{
    return 0;
}