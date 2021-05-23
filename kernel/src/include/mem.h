#ifndef __MEM_H
#define __MEM_H

#include <stdint.h>
#include <types.h>
namespace myos
{
    namespace hardware
    {
        
        class MemHandlerBaseClass
        {

            uintptr_t mmio_address;

        public:
            MemHandlerBaseClass(uintptr_t base);
            ~MemHandlerBaseClass();

        void outb(uintptr_t offset, uint8_t value)
        {
            mem_outb(offset + mmio_address, value);
        }

        void outw(uintptr_t offset, uint16_t value)
        {
            mem_outw(offset  + mmio_address, value);
        }

        void outl(uintptr_t offset, uint32_t value)
        {
            mem_outl(offset + mmio_address, value);
        }

        void outq(uintptr_t offset, uint32_t value)
        {
            mem_outq(offset + mmio_address, value);
        }

        uint8_t inb(uintptr_t offset)
        {
            return mem_inb(offset + mmio_address);
        }

        uint16_t inw(uintptr_t offset)
        {
            return mem_inw(offset + mmio_address);
        }

        uint32_t inl(uintptr_t offset)
        {
            return mem_inl(offset + mmio_address);
        }
        uint32_t inq(uintptr_t offset)
        {
            return mem_inq(offset + mmio_address);
        }


        };
    }
}

#endif