
#ifndef __MYOS__NET__ETHERFRAME_H
#define __MYOS__NET__ETHERFRAME_H

#include <types.h>
#include <stdint.h>
#include <mmu_heap.h>
#include <drivers/net/net.h>

//65535
#define MAX_PORTS 0xff
namespace myos
{
    namespace net
    {

        struct EtherFrameHeader
        {
            uint64_t dstMAC_BE : 48;
            uint64_t srcMAC_BE : 48;
            uint16_t etherType_BE;
        } __attribute__((packed));

        typedef uint32_t EtherFrameFooter;

        class EtherFrameProvider;

        class EtherFrameHandler
        {
        protected:
            EtherFrameProvider *backend;
          uint16_t etherType_BE;

        public:
            EtherFrameHandler(EtherFrameProvider *backend, uint16_t etherType);
            ~EtherFrameHandler();

            virtual bool OnEtherFrameReceived(uint8_t *etherframePayload, uint32_t size);
            void Send(uint64_t dstMAC_BE, uint8_t *etherframePayload, uint32_t size);
            uint32_t GetIPAddress();
        };

        class EtherFrameProvider 
        {
            friend class EtherFrameHandler;
            myos::drivers::NetDataHandlerBaseClass *backend;

        protected:
            EtherFrameHandler *handlers[MAX_PORTS];

        public:
            EtherFrameProvider(myos::drivers::NetDataHandlerBaseClass *backend);
            ~EtherFrameProvider();

            bool OnRawDataReceived(uint8_t *buffer, uint32_t size);
            void Send(uint64_t dstMAC_BE, uint16_t etherType_BE, uint8_t *buffer, uint32_t size);

            uint64_t GetMACAddress();
            uint32_t GetIPAddress();
        };

    } // namespace net
} // namespace myos

/* linux 源码 */

#define ETH_ALEN 6         /* Octets in one ethernet addr	 */
#define ETH_HLEN 14        /* Total octets in header.	 */
#define ETH_ZLEN 60        /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN 1500  /* Max. octets in payload	 */
#define ETH_FRAME_LEN 1514 /* Max. octets in frame sans FCS */

/* These are the defined Ethernet Protocol ID's. */
#define ETH_P_LOOP 0x0060  /* Ethernet Loopback packet	*/
#define ETH_P_ECHO 0x0200  /* Ethernet Echo packet		*/
#define ETH_P_PUP 0x0400   /* Xerox PUP packet		*/
#define ETH_P_IP 0x0800    /* Internet Protocol packet	*/
#define ETH_P_ARP 0x0806   /* Address Resolution packet	*/
#define ETH_P_RARP 0x8035  /* Reverse Addr Res packet	*/
#define ETH_P_X25 0x0805   /* CCITT X.25			*/
#define ETH_P_ATALK 0x809B /* Appletalk DDP		*/
#define ETH_P_AARP 0x80F3  /* Appletalk AARP		*/
#define ETH_P_IPX 0x8137   /* IPX over DIX			*/
#define ETH_P_802_3 0x0001 /* Dummy type for 802.3 frames  */
#define ETH_P_AX25 0x0002  /* Dummy protocol id for AX.25  */
#define ETH_P_ALL 0x0003   /* Every packet (be careful!!!) */
#define ETH_P_802_2 0x0004 /* 802.2 frames 		*/
#define ETH_P_SNAP 0x0005  /* Internal only		*/
/* This is an Ethernet frame header. */
struct ethhdr
{
    unsigned char h_dest[ETH_ALEN];   /* destination eth addr	*/
    unsigned char h_source[ETH_ALEN]; /* source ether addr	*/
    unsigned short h_proto;           /* packet type ID field	*/
};

/* Ethernet statistics collection data. */
struct enet_statistics
{
    int rx_packets; /* total packets received	*/
    int tx_packets; /* total packets transmitted	*/
    int rx_errors;  /* bad packets received		*/
    int tx_errors;  /* packet transmit problems	*/
    int rx_dropped; /* no space in linux buffers	*/
    int tx_dropped; /* no space available in linux	*/
    int multicast;  /* multicast packets received	*/
    int collisions;

    /* detailed rx_errors: */
    int rx_length_errors;
    int rx_over_errors;   /* receiver ring buff overflow	*/
    int rx_crc_errors;    /* recved pkt with crc error	*/
    int rx_frame_errors;  /* recv'd frame alignment error */
    int rx_fifo_errors;   /* recv'r fifo overrun		*/
    int rx_missed_errors; /* receiver missed packet	*/

    /* detailed tx_errors */
    int tx_aborted_errors;
    int tx_carrier_errors;
    int tx_fifo_errors;
    int tx_heartbeat_errors;
    int tx_window_errors;
};

#endif
