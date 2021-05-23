#ifndef __INTELHD_H
#define __INTELHD_H

#include <sound/sound.h>   
#include <stdint.h>
#include <pci.h>
#include <mem.h>


namespace myos
{
    namespace drivers
    {
            
        class intelHDHandler :  public SoundHandlerBaseClass
        {           
        public:
            bool SoundUp();
            bool SoundDown();
            bool SoundOpen();
            bool SoundStop();

          
            ~intelHDHandler();
        };

        class IntelHDSound : public Driver, public hardware::InterruptHandler
        {
            SoundHandlerBaseClass* SoundHandler;
           
            public:
                uint32_t mmio_address;
                void HandleInterrupt(isr_ctx_t *regs);

                IntelHDSound(myos::drivers::PeripheralComponentInterconnectDeviceDescriptor *dev,
                        myos::hardware::InterruptManager* interrupts);
                ~IntelHDSound();

                hardware::MemHandlerBaseClass memhandler;
                void Activate();
                bool Reset();
                void SetHandler(SoundHandlerBaseClass* handler);

                uint32_t corb_buffer[1024 + 128];
                uint32_t* corb_point;

                uint32_t rirb_buffer[2048 + 128] ;
                uint32_t out_stream[1024 + 128];
                uint32_t* in_stream;
                
                
        };
    }
}


#endif