
#ifndef __MYOS__DRIVERS__INTEL_82545_H
#define __MYOS__DRIVERS__INTEL_82545_H

#include <stdint.h>
#include <driver.h>
#include <isr.h>
#include <pci.h>
#include <drivers/net/net.h>
#include <mem.h>

#define E1000_REG_RCTL_EN 1 << 1
#define E1000_REG_RXDCTL_ENABLE 1 << 25

#define E1000_REG_RCTL_BSIZE1 ((1 << 16) | (1 << 17)) // 256 bytes or 4096
#define E1000_REG_RCTL_BSIZE2 ((0 << 16) | (1 << 17)) // 512 bytes or 8192
#define E1000_REG_RCTL_BSIZE3 ((1 << 16) | (0 << 17)) // 1024 bytes or 16384
#define E1000_REG_RCTL_BSIZE4 ((0 << 16) | (0 << 17)) // 2048 bytes or reverved

#define E1000_REG_TCTL_PSP 1 << 3

#define RCTL_EN (1 << 1)
#define RCTL_SBP (1 << 2)
#define RCTL_UPE (1 << 3)
#define RCTL_MPE (1 << 4)
#define RCTL_LPE (1 << 5)
#define RDMTS_HALF (0 << 8)
#define RDMTS_QUARTER (1 << 8)
#define RDMTS_EIGHTH (2 << 8)
#define RCTL_BAM (1 << 15)
#define RCTL_BSIZE_256 (3 << 16)
#define RCTL_BSIZE_512 (2 << 16)
#define RCTL_BSIZE_1024 (1 << 16)
#define RCTL_BSIZE_2048 (0 << 16)
#define RCTL_BSIZE_4096 ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192 ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384 ((1 << 16) | (1 << 25))
#define RCTL_SECRC (1 << 26)

#define E1000_REG_IMS 0xd0
#define E1000_REG_ICR 0xc0

#define E1000_REG_TCTL_PSP 1 << 3
#define E1000_REG_TCTL_EN 1 << 1

#define E1000_REG_IMS_LSC 1 << 2
#define E1000_REG_IMS_RXO 1 << 6
#define E1000_REG_IMS_RXT 1 << 7
#define E1000_REG_IMS_TXDW 1 << 0
#define E1000_REG_IMS_TXQE 1 << 1

#define E1000_REG_ICR_LSC 1 << 2

#define E1000_REG_ICR_TXQE 1 << 1
#define E1000_REG_ICR_TXDW 1 << 0

#define E1000_REG_ICR_RXT 1 << 7
#define E1000_REG_ICR_RXO 1 << 6

#define E1000_RXDESC_NR 256
#define E1000_TXDESC_NR 256

#define E1000_IOBUF_SIZE 2048

#define E1000_TX_CMD_EOP 1 << 0
#define E1000_TX_CMD_FCS 1 << 1
#define E1000_TX_CMD_RS 1 << 3

#define E1000_RX_STATUS_EOP 1 << 1
#define E1000_RX_STATUS_DONE 1 << 0

#define E1000_REG_RCTL_UPE 1 << 3
#define E1000_REG_RCTL_MPE 1 << 4
#define E1000_REG_RCTL_BAM 1 << 15
#define E1000_RCTL_SECRC 1 << 26

#define STATUS_SPEED (3 << 6)

#define SPEED_10 0
#define SPEED_100 1
#define SPEED_1000 2
#define SPEED_1000_ALT 3

namespace myos
{
    namespace drivers
    {
 

#define NUM_RX_DESCRIPTORS 76
#define NUM_TX_DESCRIPTORS 76

        struct Intel_82545_Addr
        {
            uint64_t MAC : 48;
            uint64_t logicalAddress;
        } __attribute__((packed));

     
            // RX and TX descriptor structures
            typedef struct __attribute__((packed)) i825xx_rx_desc_s
            {
                uint64_t address;

                uint16_t Length;
                uint16_t Checksum;
                uint8_t Status;
                uint8_t Errors;
                uint16_t Special;

            } i825xx_rx_desc_t;

            typedef struct __attribute__((packed)) i825xx_tx_desc_s
            {
                uint64_t address;

                uint16_t Length;
                uint8_t ChecksumOff;
                uint8_t Command;
                uint8_t Status;
                uint8_t ChecksumSt;
                uint16_t special;

            } i825xx_tx_desc_t;

            // Device-specific structure
            typedef struct i825xx_device_s
            {        
                volatile i825xx_rx_desc_t rx_desc[NUM_RX_DESCRIPTORS]; // receive descriptor buffer
                volatile uint16_t rx_tail;
               
                volatile i825xx_tx_desc_t tx_desc[NUM_TX_DESCRIPTORS]  ; // transmit descriptor buffer
                volatile uint16_t tx_tail;
                uint32_t TxBufferSize;
                uint32_t RxBufferSize;
       
         //       uint16_t (*eeprom_read)(struct i825xx_device_s *, uint8_t);
            } i825xx_device_t;
            class Intel_82545 : public Driver, public myos::hardware::InterruptHandler, public drivers::NetDataHandlerBaseClass
            {

                uint8_t *RxBuffer;
                uint8_t *TxBuffer;

                volatile uint8_t *rx_desc_base;
                volatile uint8_t *tx_desc_base;

                i825xx_device_s *e1000;

                Intel_82545_Addr intel_82545_addr;

                myos::hardware::MemHandlerBaseClass memhandle;
                uint8_t interruptLine;
            public:
                Intel_82545(uintptr_t BaseAddress, uint8_t interruptLine,
                            myos::hardware::InterruptManager *interrupts);
                ~Intel_82545();

                void Activate();
                bool Reset();
                void HandleInterrupt(isr_ctx_t *regs);
                void DoSendMsg(uint8_t *msg, uint32_t size);
                bool Send(uint8_t *buffer, uint32_t size);
                bool Receive();

                uintptr_t GetMACAddress();
                void SetIPAddress(uint32_t);
                uintptr_t GetIPAddress();

                void net_i825xx_rx_enable();
                bool net_i825xx_rx_init();
                bool net_i825xx_tx_init();
                void net_i825xx_tx_poll(void *pkt, uint16_t length);
                void net_i825xx_rx_poll();
                uint16_t net_i825xx_phy_read(int MDIC_REGADD);

                int virnet_i825xx_rx_init();
                int virnet_i825xx_tx_init();

                uint32_t prom_read(uint32_t order);

                void e1000SetRegister(int Reg, int Value);
                void e1000UnsetRegister(int Reg, int Value);

                uint32_t GetSpeed();
                Driver* GetDriver();
            };

    } // namespace drivers
} // namespace myos

#endif
