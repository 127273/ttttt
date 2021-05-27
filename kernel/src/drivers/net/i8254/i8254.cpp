#include "i8254.h"
#include <memory.h>
#include <kernel.h>
#include <mmu_heap.h>

using namespace myos;
using namespace myos::drivers;
using namespace myos::hardware;


#define E1000_REG_CTRL 0x00
#define E1000_REG_STATUS 0x08
#define E1000_REG_EERD 0x14
#define E1000_REG_FCAL 0x28
#define E1000_REG_FCAH 0x2c
#define E1000_REG_FCT 0x30
#define E1000_REG_RCTL 0x100
#define E1000_REG_FCTTV 0x170
#define E1000_REG_TCTL 0x400
#define E1000_REG_CRCERRS 0x4000
#define E1000_REG_RAL 0x5400
#define E1000_REG_MTA 0x5200
#define E1000_REG_RAH 0x5404

#define E1000_REG_RXDCTL 0x2828
#define E1000_REG_TDBAL 0x3800
#define E1000_REG_TDBAH 0x3804
#define E1000_REG_TDLEN 0x3808
#define E1000_REG_TDH 0x3810
#define E1000_REG_TDT 0x3818
#define E1000_REG_RDH 0x2810
#define E1000_REG_RDT 0x2818
#define E1000_REG_RDTR 0x2820

#define E1000_REG_CTRL_RST 1 << 26
#define E1000_REG_CTRL_ASDE 1 << 5
#define E1000_REG_CTRL_SLU 1 << 6
#define E1000_REG_CTRL_LRST 1 << 3
#define E1000_REG_CTRL_PHY_RST int(1 << 31)
#define E1000_REG_CTRL_ILOS 1 << 7
#define E1000_REG_CTRL_VME 1 << 30

#define E1000_REG_EERD_START 1 << 0
#define E1000_REG_EERD_DATA 0xffff << 16

#define E1000_REG_RAH_AV int(1 << 31)

#define E1000_REG_RDBAL 0x2800
#define E1000_REG_RDBAH 0x2804
#define E1000_REG_RDLEN 0x2808



Driver* Intel_82545::GetDriver()
{
    return (Driver*)this;
}

uint32_t Intel_82545::prom_read(uint32_t order)
{

    memhandle.outl(E1000_REG_EERD, 1 | (order << 8));
    while ((memhandle.inl(E1000_REG_EERD) & 1 << 4) != 1 << 4)
        ;
    uint32_t k = memhandle.inl(E1000_REG_EERD);
    return k;
}

void Intel_82545::net_i825xx_rx_enable()
{
    //i825xx_device_t *dev = (i825xx_device_t *)netdev->lldev;
    memhandle.outl(E1000_REG_RCTL, memhandle.inl(E1000_REG_RCTL) | (RCTL_EN));
}

void Intel_82545::e1000SetRegister(int Reg, int Value)
{
    int data = memhandle.inl(Reg);
    memhandle.outl(Reg, data | Value);
}

void Intel_82545::e1000UnsetRegister(int Reg, int Value)
{
    int data = memhandle.inl(Reg);
    memhandle.outl(Reg, data & Value);
}

bool Intel_82545::net_i825xx_rx_init()
{
    int i;

    // unaligned base address
    uint64_t tmpbase = (uint64_t)malloc((sizeof(i825xx_rx_desc_t) * NUM_RX_DESCRIPTORS) + 16);
    // aligned base address

    rx_desc_base = (tmpbase % 16) ? (uint8_t *)(tmpbase + 16 - (tmpbase % 16)) : (uint8_t *)tmpbase;

    for (i = 0; i < NUM_RX_DESCRIPTORS; i++)
    {
      //  e1000->rx_desc[i] = (i825xx_rx_desc_t *)(e1000->rx_desc_base + (i * sizeof(i825xx_rx_desc_s)));
        e1000->rx_desc[i].address = 0; // packet buffer size (8K)
        e1000->rx_desc[i].Status = 0;
        //e1000->rx_desc[i]->Checksum = 0;
        //e1000->rx_desc[i]->Errors = 0;
        //e1000->rx_desc[i]->Length = 0;
        //e1000->rx_desc[i]->Special = 0;
    }

    e1000->RxBufferSize = NUM_RX_DESCRIPTORS * E1000_IOBUF_SIZE;
    RxBuffer = (uint8_t *)malloc(e1000->RxBufferSize + 16);

    RxBuffer = (uint8_t *)(uint64_t(RxBuffer) + 16 - (uint64_t(RxBuffer) % 16));

    for (i = 0; i < NUM_TX_DESCRIPTORS; i++)
    {
        e1000->rx_desc[i].address = uint64_t(uint64_t(RxBuffer) + (i * E1000_IOBUF_SIZE));
        //e1000->rx_desc[i]->Status = 0;
    }

    // setup the receive descriptor ring buffer (TODO: >32-bits may be broken in this code)
    memhandle.outl(E1000_REG_RDBAH, (uint32_t)((uint64_t)rx_desc_base >> 32));
    memhandle.outl(E1000_REG_RDBAL, (uint32_t)((uint64_t)rx_desc_base & 0xFFFFFFFF));
    DEBUG("rx_init RDBAH/RDBAL = %x:%x\n", memhandle.inq(E1000_REG_RDBAH), memhandle.inq(E1000_REG_RDBAL));

    // receive buffer length; NUM_RX_DESCRIPTORS 16-byte descriptors
    memhandle.outl(E1000_REG_RDLEN, (uint32_t)(NUM_RX_DESCRIPTORS * sizeof(i825xx_rx_desc_s)));

    // setup head and tail pointers
    memhandle.outl(E1000_REG_RDH, 0);
    memhandle.outl(E1000_REG_RDT, NUM_RX_DESCRIPTORS - 1);
    e1000->rx_tail = 0;
    // No delay time for reception ints
    memhandle.outl(E1000_REG_RDTR, 0);

    // set packet size
    //	e1000UnsetRegister(E1000_REG_RCTL, E1000_REG_RCTL_BSIZE1);

    // No loopback
    //	e1000UnsetRegister(E1000_REG_RCTL, ((1 << 7) | (1 << 6)));

    // set the receieve control register (promisc ON, 8K pkt size)
    memhandle.outl(E1000_REG_RCTL, RCTL_SBP | RCTL_UPE | RCTL_MPE | RDMTS_HALF | RCTL_SECRC |
                                       RCTL_LPE | RCTL_BAM | RCTL_BSIZE_8192 | 1 << 1 | 1 << 23);

    return true;
}

#define TCTL_EN (1 << 1)
#define TCTL_PSP (1 << 3)

bool Intel_82545::net_i825xx_tx_init()
{
    int i;

    uint64_t tmpbase = (uint64_t)malloc((sizeof(i825xx_tx_desc_t) * NUM_TX_DESCRIPTORS) + 16);


    DEBUG("%x", tmpbase);

    tx_desc_base = (tmpbase % 16) ? (uint8_t *)((tmpbase) + 16 - (tmpbase % 16)) : (uint8_t *)tmpbase;

    for (i = 0; i < NUM_TX_DESCRIPTORS; i++)
    {
     //   e1000->tx_desc[i] = (i825xx_tx_desc_t *)(e1000->tx_desc_base + (i * 16));
        e1000->tx_desc[i].address = 0;
        e1000->tx_desc[i].Command = 0;
        //e1000->tx_desc[i]->ChecksumOff = 0;
        //e1000->tx_desc[i]->ChecksumSt = 0;
        //e1000->tx_desc[i]->special = 0;
        //e1000->tx_desc[i]->Length = 0;
        //e1000->tx_desc[i]->Status = 0;
    }

    e1000->TxBufferSize = NUM_RX_DESCRIPTORS * E1000_IOBUF_SIZE;
    TxBuffer = (uint8_t *)malloc(e1000->TxBufferSize + 16);

    TxBuffer = (uint8_t *)(uintptr_t(TxBuffer) + 16 - uintptr_t(TxBuffer) % 16);

    for (i = 0; i < NUM_TX_DESCRIPTORS; i++)
    {
        e1000->tx_desc[i].Command = 0;
        e1000->tx_desc[i].address = uint64_t(uint64_t(TxBuffer) + (i * E1000_IOBUF_SIZE));
    }
    DEBUG("ok");
    // setup the transmit descriptor ring buffer
    memhandle.outl(E1000_REG_TDBAH, (uint32_t)((uint64_t)tx_desc_base >> 32));
    memhandle.outl(E1000_REG_TDBAL, (uint32_t)((uint64_t)tx_desc_base & 0xFFFFFFFF));
    DEBUG("tx_init TDBAH/TDBAL = %x:%x\n", memhandle.inl(E1000_REG_TDBAH), memhandle.inl(E1000_REG_TDBAL));

    // transmit buffer length; NUM_TX_DESCRIPTORS 16-byte descriptors
    memhandle.outl(E1000_REG_TDLEN, (uint32_t)(NUM_TX_DESCRIPTORS * 16));

    // setup head and tail pointers
    memhandle.outl(E1000_REG_TDH, 0);
    memhandle.outl(E1000_REG_TDT, NUM_RX_DESCRIPTORS - 1);

    e1000->tx_tail = 0;
    /*Desc.Status  := E1000_RX_STATUS_DONE;1 << 0
  Desc.Length  := net.OutgoingPackets.size;
  Desc.Command := E1000_TX_CMD_EOP or E1000_TX_CMD_FCS or E1000_TX_CMD_RS; 0  1 3
  E1000WriteRegister(@NicE1000, E1000_REG_TDT,  Next);
  */
    // set the transmit control register (padshortpackets)
    memhandle.outl(E1000_REG_TCTL, (TCTL_EN | TCTL_PSP));
    return true;
}

void Intel_82545::net_i825xx_tx_poll(void *pkt, uint16_t length)
{

    //printf("i825xx[%u]: transmitting packet (%u bytes) [h=%u, t=%u]\n", netdev->hwid, length, mmio_read32(i825xx_REG_TDH), e1000->tx_tail);
    //e1000->tx_desc[e1000->tx_tail]->address = (uint64_t)pkt;
    e1000->tx_desc[e1000->tx_tail].Length = length;
    e1000->tx_desc[e1000->tx_tail].Command = ((1 << 3) | (3));

    // update the tail so the hardware knows it's ready
    int oldtail = e1000->tx_tail;
    e1000->tx_tail = (e1000->tx_tail + 1) % NUM_TX_DESCRIPTORS;
    memhandle.outl(E1000_REG_TDT, e1000->tx_tail);

    while (!(e1000->tx_desc[oldtail].Status & 0xF))
    {
    }

    //printf("i825xx[%u]: transmit status = 0x%p1x\n", netdev->hwid, (e1000->tx_desc[oldtail]->sta & 0xF));
}

// This can be used stand-alone or from an interrupt handler
void Intel_82545::net_i825xx_rx_poll()
{

    while ((e1000->rx_desc[e1000->rx_tail].Status & (1 << 0)))
    {
        // raw packet and packet length (excluding CRC)
        //uint8_t *pkt = (void *)dev->rx_desc[dev->rx_tail]->address;
        DEBUG("rx poll  ");
        uint8_t *pkt = (uint8_t *)e1000->rx_desc[e1000->rx_tail].address;
        uint16_t pktlen = e1000->rx_desc[e1000->rx_tail].Length;
        bool dropflag = false;

        if (pktlen < 60)
        {
            DEBUG("net[u]: short packet (%u bytes)\n", pktlen);
            dropflag = true;
        }

        // while not technically an error, there is no support in this driver
        if (!(e1000->rx_desc[e1000->rx_tail].Status & (1 << 1)))
        {
            DEBUG("no EOP set! (len=%u, 0x%x 0x%x 0x%x)\n",
                  pktlen, pkt[0], pkt[1], pkt[2]);
            dropflag = true;
        }

        if (e1000->rx_desc[e1000->rx_tail].Errors)
        {
            DEBUG("rx errors (0x%x)\n", e1000->rx_desc[e1000->rx_tail].Errors);
            dropflag = true;
        }

        if (!dropflag)
        {
            DEBUG("size %x  ", e1000->rx_desc[e1000->rx_tail].Length);
            // send the packet to higher layers for parsing
            uint32_t size = e1000->rx_desc[e1000->rx_tail].Length;

            uint8_t *buffer = (uint8_t *)(RxBuffer + e1000->rx_tail * E1000_IOBUF_SIZE);

            for (int i = 0; i < 42; i++)
            {
                DEBUG("%x ", buffer[i]);
            }
        }

        // update RX counts and the tail pointer

        e1000->rx_desc[e1000->rx_tail].Status = (uint16_t)(0);

        e1000->rx_tail = (e1000->rx_tail + 1) % NUM_RX_DESCRIPTORS;
        // write the tail to the device
        //e1000->rx_tail = (e1000->rx_tail + 1) % NUM_RX_DESCRIPTORS;
        memhandle.outl(E1000_REG_RDT, e1000->rx_tail);
    }
}

Intel_82545::Intel_82545(uintptr_t BaseAddress, uint8_t interruptLine, InterruptManager *interrupts)
    : Driver(),
      InterruptHandler(interrupts, interruptLine + 0x20),
      memhandle(BaseAddress)
{

   
    this->interruptLine = interruptLine;
    //   DEBUG("dev, %d", uint8_t(interruptLine + 0x20));
    //while(1);
    // reset   mem_outl( mmio_address,mem_inl(mmio_address ) | (1 << 6) );
    memhandle.outl(E1000_REG_CTRL, E1000_REG_CTRL_RST);

    for (int i = 0; i < 3000; i++)
        wait_ms(1);
    //while((mem_read32(E1000_REG_CTRL) & E1000_REG_CTRL_RST) == E1000_REG_CTRL_RST);

    // Link is set up
    e1000SetRegister(E1000_REG_CTRL, E1000_REG_CTRL_ASDE | E1000_REG_CTRL_SLU);
    e1000UnsetRegister(E1000_REG_CTRL, E1000_REG_CTRL_LRST);
    e1000UnsetRegister(E1000_REG_CTRL, E1000_REG_CTRL_PHY_RST);
    e1000UnsetRegister(E1000_REG_CTRL, E1000_REG_CTRL_ILOS);

    // Flow control is disabled
    // TODO: qemu logs says this is invalid write
    memhandle.outl(E1000_REG_FCAL, 0);
    memhandle.outl(E1000_REG_FCAH, 0);
    memhandle.outl(E1000_REG_FCT, 0);
    memhandle.outl(E1000_REG_FCTTV, 0);

    // VLAN is disable
    e1000UnsetRegister(E1000_REG_CTRL, E1000_REG_CTRL_VME);
   
    // Initialize statistics registers
    for (int I = 0; I < 64; I++)
        memhandle.outl(E1000_REG_CRCERRS + (I * 4), 0);

    //reset
    //  mem_write8(dev->mmap + 0x3, 1 << 2);
    uint32_t temp = memhandle.inl(0);
    temp &= (~(0xff));
    temp |= (1 << 2);
    memhandle.outb(0x3, 1 << 2);
    while (memhandle.inl(0) & (1 << 26))
        ;

    // while(mem_readb(dev->mmap + 0x3) & (1 << 2));
    uint64_t MAC0;
    uint64_t MAC1;
    uint64_t MAC2;
    uint64_t MAC3;
    uint64_t MAC4;
    uint64_t MAC5;

    uint32_t k = prom_read(0);
    //DEBUG("%x ", k >> 16);
    MAC0 = (k >> 16) % 256;
    MAC1 = (k >> 16) / 256;
    k = prom_read(1);
    //DEBUG("%x ", k);

    MAC2 = (k >> 16) % 256;
    MAC3 = (k >> 16) / 256;

    k = prom_read(2);
    //DEBUG("%x ", k);
 
    MAC4 = (k >> 16) % 256;
    MAC5 = (k >> 16) / 256;

    intel_82545_addr.MAC = MAC5 << 40 | MAC4 << 32 | MAC3 << 24 | MAC2 << 16 | MAC1 << 8 | MAC0;

    DEBUG("intel_82545 MAC address  ");
    DEBUG("%x:%x:%x;%x;%x \n", MAC0, MAC1, MAC2, MAC3, MAC4, MAC5);

    uint32_t IPV4;
    uint16_t k1;
    k1 = (prom_read(0x25) >> 16);
    IPV4 = (k1 >> 8) | ((k1 & 0xff) << 8);
    DEBUG("k1: %d  %d ", k1 >> 8, (k1 & 0xff) << 8);
    DEBUG("%x", IPV4);

    k1 = (prom_read(0x26) >> 16);
    IPV4 |= (k1 >> 16) | ((k1 & 0xffff) << 16);
    DEBUG("k1: %d  %d ", k1 >> 8, (k1 & 0xff) << 8);
    DEBUG("IPV4%x \n", IPV4);

    /*  General Registers: Reset to power-on values.
            CTRL, STATUS, EECD, CTRL_EXT, FCAL, FCAH, FCT, VET, FCTTV, TXCW, RXCW, PBA,
        Interrupt Registers: Reset to power-on values.
        Receive Registers: Reset to power-on values (exceptions are the RAH/RAL, MTA, VFTA 
            and RDBAH/RDBAL registers, which are not reset to any preset 
            value. The valid bit of the RAH register is cleared).
        Transmit Registers: Reset to power-on values (exceptions are the TDBAH/TDBAL registers, which are not reset to any preset value).
        Statistics Registers: Reset to power-on values.
        Wakeup Registers: The WUC (except for the PME_En and PME_Status bits if 
            AUX_POWER = 1b), WUFC, IPAV, and FFLT registers are reset to 
            their default value.
        Diagnostic Registers: Reset to power-on values (exception is the PBM memory, which is not 
            reset to any preset value).
        PCI Config Space: Context Lost; requires initialization.
        PHY: RST# is asserted to reset the PHY while LAN_PWR_GOOD is deasserted.
    */
   e1000 = (i825xx_device_t *)malloc(sizeof(i825xx_device_t));

    net_i825xx_tx_init();
    net_i825xx_rx_init();
 
}

Intel_82545::~Intel_82545()
{
}

void Intel_82545::Activate()
{
    //DEBUG("intel_82545 Activate");
    // Set receive address
    /* 激活中断 */
  //  irq_enable(interruptLine);
    uint32_t status = memhandle.inl(0xc0);

    DEBUG("intel  net Activate %x\n", status);
    //link reset
    memhandle.outl(E1000_REG_CTRL, memhandle.inl(E1000_REG_CTRL) | (1 << 6));
    //active recivce
    memhandle.outl(E1000_REG_RCTL, memhandle.inl(E1000_REG_RCTL) | (1 << 1));

    memhandle.outl(0x00d0, 0x1F6DF);
    memhandle.inl(0xC0);

    memhandle.outl(E1000_REG_RAL, (uint32_t)(intel_82545_addr.MAC));
    memhandle.outl(E1000_REG_RAH, (uint32_t)(intel_82545_addr.MAC >> 32));
    e1000SetRegister(E1000_REG_RAH, E1000_REG_RAH_AV);

    // Clear Multicast Table Array (MTA)
    for (int I = 0; I < 128; I++)
        memhandle.outl(E1000_REG_MTA + (I * 4), 0);

    // enable interrupt
    e1000SetRegister(E1000_REG_IMS, E1000_REG_IMS_LSC | E1000_REG_IMS_RXO | E1000_REG_IMS_RXT | E1000_REG_IMS_TXQE | E1000_REG_IMS_TXDW);
    // clear any spurius irq
    memhandle.inl(E1000_REG_ICR);

    // get link status
    int link = memhandle.inl(E1000_REG_STATUS) | (1 << 1);
    if (link)
    {
        DEBUG("[i8254x] Link %s, Speed : %d ", link ? " Up " : " Down ", GetSpeed());
    }
}

bool Intel_82545::Reset()
{
    return true;
}

uint32_t Intel_82545::GetSpeed()
{
    int spd = memhandle.inl(E1000_REG_STATUS) & STATUS_SPEED;
    spd >>= 6;

    switch (spd)
    {
    case SPEED_10:
        spd = 10;
        break;
    case SPEED_100:
        spd = 100;
        break;
    case SPEED_1000:
    case SPEED_1000_ALT:
        spd = 1000;
        break;
    default:
        spd = 0;
        break;
    }

    return spd;
}

#define MDIC_PHYADD (1 << 21)
#define MDIC_OP_WRITE (1 << 26)
#define MDIC_OP_READ (2 << 26)
#define MDIC_R (1 << 28)
#define MDIC_I (1 << 29)
#define MDIC_E (1 << 30)

uint16_t Intel_82545::net_i825xx_phy_read(int MDIC_REGADD)
{
    uint16_t MDIC_DATA = 0;

    memhandle.outl(0x20, (((MDIC_REGADD & 0x1F) << 16) |
                          MDIC_PHYADD | MDIC_I | MDIC_OP_READ));

    while (!(memhandle.inl(0x20) & (MDIC_R | MDIC_E)))
    {
        wait_ms(1);
    }

    if (memhandle.inl(0x20) & MDIC_E)
    {
        DEBUG("i825xx: MDI READ ERROR\n");
        return -1;
    }

    MDIC_DATA = (uint16_t)(memhandle.inl(0x20) & 0xFFFF);
    return MDIC_DATA;
}

void Intel_82545::DoSendMsg(uint8_t *msg, uint32_t size)
{
    //	asm volatile ("pushf");

  
    e1000->tx_desc[e1000->tx_tail].Length = size;
    e1000->tx_desc[e1000->tx_tail].Command = (1 << 0 | 1 << 1 | 1 << 3);
    e1000->tx_desc[e1000->tx_tail].Status = 1 << 0;
    e1000->tx_tail++;
    memhandle.outl(E1000_REG_TDT, e1000->tx_tail);
    //asm volatile ("popf");
}

void Intel_82545::HandleInterrupt(isr_ctx_t *regs)
{
        /* ok 我们需要停止一下 它一直在中断 */
        
        uint32_t icr = memhandle.inl(E1000_REG_ICR);
        DEBUG("intel82545 handleinterrupt icr %x\n", icr);
        printk("intel82545 handleinterrupt\n");
        // LINK STATUS CHANGE
        if (icr & (1 << 2))
        {

            memhandle.outl(E1000_REG_CTRL, (memhandle.inl(E1000_REG_CTRL) | (1 << 6)));

            DEBUG("i825xx: Link Status Change, STATUS = 0x%x\n", memhandle.inl(0x8));
        }

        // RX underrun / min threshold
        if (icr & (1 << 6) || icr & (1 << 4))
        {
            //icr &= ~((1 << 6) | (1 << 4));
            DEBUG(" underrun (rx_head = %x, )", memhandle.inl(0x2810));
        }
        //while((mem_read32(mmio_address + E1000_REG_ICR) &  ((1 << 6) | (1 << 7))) == 0);

        if (icr & (1 << 7))
        {
            Receive();
            //net_i825xx_rx_poll();
        }

        if (icr & ((1 << 1) | (1 << 0)))
        {
            DEBUG("e1000: Packet transmitted \n");

            //DoSendMsg(txbuffer, 0x2a);
        }

        memhandle.inl(0xC0);
        //pic_acknowledge(PIC_IRQC);
     
        // clearing the pending interrupts
        
        //DEBUG("over");
        //
        // asm volatile("sti");
    }

    bool Intel_82545::Send(uint8_t * buffer, uint32_t size)
    {
        DEBUG("intel Send  ");
        //printfHex16(size);
        if (size > E1000_IOBUF_SIZE)
            size = E1000_IOBUF_SIZE;

        for (int i = 0; i < (size > 64 ? 64 : size); i++)
        {
            //  printfHex(buffer[i]);
            DEBUG("%x ", buffer[i]);
            // int x = (int)buffer[i] % 16;
            // int y = (int)buffer[i] / 16;
            //   putchar((char)y);
            // putchar(x);
          
        }
        //printk("asiasns");
        uint32_t Head = memhandle.inl(E1000_REG_TDH);
        e1000->tx_tail = memhandle.inl(E1000_REG_TDT);
        uint32_t Next = (e1000->tx_tail + 1) % NUM_RX_DESCRIPTORS;

        //uint8_t *src = (uint8_t *)malloc(sizeof(0x100));
        //uint8_t *temp = (uint8_t *)malloc(sizeof(0x100));
        //uint8_t *dst = (uint8_t *)malloc(sizeof(0x100));
        DEBUG("Tail %d", e1000->tx_tail);

        for (uint8_t *src = buffer + size - 1, *dst = (uint8_t *)(TxBuffer + e1000->tx_tail * E1000_IOBUF_SIZE + size - 1); src >= buffer; src--, dst--)
        {
            *dst = *src;
            // *temp = *src;
        }

        //e1000->tx_tail = dev->rx_tail;
        //dev->rx_tail = (dev->rx_tail + 1) % NUM_RX_DESCRIPTORS;

        e1000->tx_desc[e1000->tx_tail].Length = size;
        e1000->tx_desc[e1000->tx_tail].Command = (1 << 0 | 1 << 1 | 1 << 3);
        e1000->tx_desc[e1000->tx_tail].Status = E1000_RX_STATUS_DONE;

        memhandle.outl(E1000_REG_TDT, Next);

        while (!(e1000->tx_desc[e1000->tx_tail].Status & 0xF))
        {
            // Sleep(1);
        }
        DEBUG("ok");
        return true;
    }

    bool Intel_82545::Receive()
    {
        //	while((e1000->rx_desc[Tail]->Status & E1000_RX_STATUS_DONE) )
        //	{
               int *addr = (int *)KERNEL_VIDEO_MEMORY;
                    	for(int i = 0 ;i<1440*20;i++)
                        {
                            *((char *)addr+0)=(char)0xff;
                            *((char *)addr+1)=(char)0x00;
                            *((char *)addr+2)=(char)0x00;
                            *((char *)addr+3)=(char)0x00;	
                            addr +=1;	
                        }
        DEBUG("intel_82545 RECV: ");
        bool drop = false;
        uint32_t Head = memhandle.inl(E1000_REG_RDH);
        uint32_t Tail = memhandle.inl(E1000_REG_RDT);
        uint32_t Current = (Tail + 1) % NUM_RX_DESCRIPTORS;

        DEBUG("e1000: new packet, head: %d, Cur: %d Status: %x size 0x%x\n", Head, Current, e1000->rx_desc[Current].Status, e1000->rx_desc[Current].Length);
        //   DEBUG("e1000: new packet, status: %d\n",e1000->rx_desc[Tail]->Status);
        if ((e1000->rx_desc[Current].Status & E1000_RX_STATUS_EOP) == 0)
        {
            DEBUG("e1000: new packet, E1000_RX_STATUS_EOP Exiting \n");
            drop = true;
        }

        //	Tail++;
        if (drop == false)
        {
            //for(int j = 0; j < 3; j++)
            //	{
            uint32_t size = e1000->rx_desc[Current].Length;

            uint8_t *buffer = (uint8_t *)(RxBuffer + Current * E1000_IOBUF_SIZE);

            for (int i = 0; i < 42; i++)
            {
                DEBUG("%x ", buffer[i]);
            }
        }
        // reset the descriptor
        e1000->rx_desc[Current].Status = 0;
        // incrementing the tail
        memhandle.outl(E1000_REG_RDT, Current);

        //}
        return true;
    }

    uintptr_t Intel_82545::GetMACAddress()
    {
        return intel_82545_addr.MAC;
    }

    void Intel_82545::SetIPAddress(uint32_t ip)
    {
        color_kdebug(3, "set \n");
        intel_82545_addr.logicalAddress = ip;
    }

    uintptr_t Intel_82545::GetIPAddress()
    {
        return intel_82545_addr.logicalAddress;
        return 0xdf0da8c0;
        return 0x0f02000a;
    }

    int Intel_82545::virnet_i825xx_rx_init()
    {
        return 0;
    }

    int Intel_82545::virnet_i825xx_tx_init()
    {
        int i;

        uint64_t tmpbase = (uint64_t)malloc((sizeof(i825xx_tx_desc_t) * NUM_TX_DESCRIPTORS) + 16);
        tx_desc_base = (tmpbase % 16) ? (uint8_t *)((tmpbase) + 16 - (tmpbase % 16)) : (uint8_t *)tmpbase;

        for (i = 0; i < NUM_TX_DESCRIPTORS; i++)
        {
            //  e1000->tx_desc[i] = (i825xx_tx_desc_t *)(e1000->tx_desc_base + (i * 16));
            e1000->tx_desc[i].address = 0;
            e1000->tx_desc[i].Command = 0;
            e1000->tx_desc[i].ChecksumOff = 0;
            e1000->tx_desc[i].ChecksumSt = 0;
            e1000->tx_desc[i].special = 0;
            e1000->tx_desc[i].Length = 0;
            e1000->tx_desc[i].Status = 0;
        }

        e1000->TxBufferSize = NUM_RX_DESCRIPTORS * E1000_IOBUF_SIZE;
        TxBuffer = (uint8_t *)malloc(e1000->TxBufferSize + 16);

        TxBuffer = (uint8_t *)(uintptr_t(TxBuffer) + 16 - uintptr_t(TxBuffer) % 16);

        for (i = 0; i < NUM_TX_DESCRIPTORS; i++)
        {
            e1000->tx_desc[i].Command = 0;
            e1000->tx_desc[i].address = uint64_t(uint64_t(TxBuffer) + (i * E1000_IOBUF_SIZE));
        }
        // setup the transmit descriptor ring buffer
        memhandle.outl(E1000_REG_TDBAH, (uint32_t)((uint64_t)tx_desc_base >> 32));
        memhandle.outl(E1000_REG_TDBAL, (uint32_t)((uint64_t)tx_desc_base & 0xFFFFFFFF));
        DEBUG("tx_init TDBAH/TDBAL = %x:%x\n", memhandle.inl(E1000_REG_TDBAH), memhandle.inl(E1000_REG_TDBAL));

        // transmit buffer length; NUM_TX_DESCRIPTORS 16-byte descriptors
        memhandle.outl(E1000_REG_TDLEN, (uint32_t)(NUM_TX_DESCRIPTORS * 16));

        // setup head and tail pointers
        memhandle.outl(E1000_REG_TDH, 0);
        memhandle.outl(E1000_REG_TDT, 0);
        e1000->tx_tail = 0;
        /*Desc.Status  := E1000_RX_STATUS_DONE;1 << 0
  Desc.Length  := net.OutgoingPackets.size;
  Desc.Command := E1000_TX_CMD_EOP or E1000_TX_CMD_FCS or E1000_TX_CMD_RS; 0  1 3
  E1000WriteRegister(@NicE1000, E1000_REG_TDT,  Next);
  */
        // set the transmit control register (padshortpackets)
        memhandle.outl(E1000_REG_TCTL, (TCTL_EN | TCTL_PSP));
        return 0;
}

NetDataHandlerBaseClass* Intel_82545::GetHandlerBaseClass()
{
    return (NetDataHandlerBaseClass*)(this);
}