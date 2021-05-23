
#ifndef __MYOS__NET__ICMP_H
#define __MYOS__NET__ICMP_H

#include <types.h>
#include <drivers/net/ipv4.h>

namespace myos
{
    namespace net
    {

        struct InternetControlMessageProtocolMessage
        {
            uint8_t type;
            uint8_t code;

            uint16_t checksum;
            uint32_t data;

        } __attribute__((packed));

        class InternetControlMessageProtocol : InternetProtocolHandler
        {
        public:
            InternetControlMessageProtocol(InternetProtocolProvider *backend);
            ~InternetControlMessageProtocol();

            bool OnInternetProtocolReceived(uint32_t srcIP_BE, uint32_t dstIP_BE,
                                            uint8_t *internetprotocolPayload, uint32_t size);
            void RequestEchoReply(uint32_t ip_be);
        };

    } // namespace net
} // namespace myos

#endif