
#ifndef __MYOS__DRIVERS__SOUND_H
#define __MYOS__DRIVERS__SOUND_H

#include <stdint.h>



#include <driver.h>
#include <pci.h>

namespace myos
{
    namespace drivers
    {
        class SoundHandlerBaseClass
        {
        public:
            virtual bool SoundUp();
            virtual bool SoundDown();
            virtual bool SoundOpen();
            virtual bool SoundStop();
            SoundHandlerBaseClass();
        };
    }
}

#endif