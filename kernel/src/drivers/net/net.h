#ifndef NET_H
#define NET_H

#include <types.h>

namespace myos
{
    namespace drivers
    {

        class NetDataHandlerBaseClass
        {

        public:
            NetDataHandlerBaseClass();

            ~NetDataHandlerBaseClass();

            virtual bool Receive();
            virtual bool Send(uint8_t *buffer, uint32_t size);
            virtual uintptr_t GetMACAddress();
            virtual uintptr_t GetIPAddress();
            virtual void SetIPAddress(uint32_t ip);
            virtual uint32_t GetSpeed();
        };

    } // namespace drivers
}

#endif // !NET_H