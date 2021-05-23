#include <drivers/usb/xchi.h>

#include <kernel.h>
#include <string.h>
#include <debug.h>
#include <memory.h>
#include <mmu_heap.h>
using namespace myos;
using namespace drivers;

struct usb_hc_t *xhci_controller;
struct xhci_hcd *controller;
/**
 * @brief 指向指针的指针
 *  int **p;
 *  int b;
 *  int *n = &b;
 *  p = &n ;
 *  *p = &b;
 * 
 *  *(controller->xdevs) = (struct xhci_dev*)(malloc())
 *  
 */


XHCIController::XHCIController(uintptr_t xhciBaseAddress, uint8_t interruptLine, myos::hardware::InterruptManager* manager)
:   memhandler(xhciBaseAddress),
    InterruptHandler(manager, interruptLine)
{
    DEBUG("%x \t", xhciBaseAddress);
    DEBUG("usb Len0x%x \n", memhandler.inb(0x00));
    xhciVirtualAddress = xhciBaseAddress;
     xhci_controller = (struct usb_hc_t *)malloc(sizeof(struct usb_hc_t));
    controller = (xhci_hcd *)malloc(sizeof(struct xhci_hcd));
    controller->cap_regs = (struct xhci_cap_regs *)(xhciVirtualAddress);
    controller->op_regs =
        (struct xhci_op_regs *)(xhciVirtualAddress+ (controller->cap_regs->caplength));
    controller->run_regs =
        (struct xhci_run_regs *)(xhciVirtualAddress + (controller->cap_regs->rtsoff));
    controller->db_regs =
        (struct xhci_db_regs *)(xhciVirtualAddress + (controller->cap_regs->dboff));
   
   
}

/**
 * @brief 
 * 
 * @param regs 
 * @return ** void 
 */
void XHCIController::HandleInterrupt(isr_ctx_t *regs)
{
    color_kdebug(KPRN_ERR, "usb interrupt\n");
    while(1);
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void XHCIController::Activate()
{
    color_kdebug(KPRN_INFO, "usb activate \n");
     // Allocate as many devices as there are slots available.
    *(controller->xdevs) = (struct xhci_dev*)(malloc(sizeof(struct xhci_dev) * (controller->cap_regs->hcsparams1 & 0xFF)));
   
    xhci_take_controller(controller);

    // TODO do this only on controller which require it
    //xhci_switch_ports(pci_dev);

      // Set up irqs
    /*controller->irq_line = get_empty_int_vector();
    if (!pci_register_msi(pci_dev, controller->irq_line)) {
        io_apic_connect_gsi_to_vec(0, controller->irq_line, pci_dev->gsi,
                                   pci_dev->gsi_flags, 1);
    }*/

    int32_t cmd = controller->op_regs->usbcmd;
    controller->op_regs->usbcmd = cmd | 1 << 1;
    
    
    // Wait for controller not ready
    // intel xhci controllers need this delay
  wait_ms(100);
    while ((controller->op_regs->usbcmd & (1 << 1))) {
    };
    while (!(controller->op_regs->usbsts & 0x00000001UL)) {
    };
    DEBUG("usb/xhci: controller halted");

    controller->op_regs->config = XHCI_CONFIG_MAX_SLOT;
    if (controller->cap_regs->addressCap64) {
        controller->context_size = 64;
    } else {
        controller->context_size = 32;
    }
    DEBUG("%d-bit Addressing Capability\t", controller->context_size);
    controller->dcbaap = (uint64_t*)malloc(2048);
    controller->dcbaap_dma = (size_t)controller->dcbaap - KERNEL_START_MEMORY;
    controller->op_regs->dcbaap = controller->dcbaap_dma;

    // Set up scratchpad_buffers
    uint32_t hcs2 = controller->cap_regs->hcsparams2;
    int spb = ((((hcs2) >> 16) & 0x3e0) | (((hcs2) >> 27) & 0x1f));
    if (spb) {
        controller->scratchpad_buffer_array = (uint64_t *)malloc(sizeof(uint64_t) * spb);
         DEBUG("usb/xhci: allocating %x scratchpad_buffers", spb);
        for (int i = 0; i < spb; i++) {
            size_t scratchpad_buffer = (size_t)malloc(0x200000);
            controller->scratchpad_buffer_array[i] =
                scratchpad_buffer - KERNEL_START_MEMORY;
        }
    }

    // the first entry in the dcbapp has to point to the buffers if they exist
    controller->dcbaap[0] =
        (size_t)controller->scratchpad_buffer_array - KERNEL_START_MEMORY;
    xhci_setup_seg(&controller->crseg, 4096, TYPE_COMMAND);
    controller->op_regs->cmdRingCtl = controller->crseg.trbs_dma | 1;
    DEBUG("usb/xhci: Initializing event ring");
    xhci_setup_seg(&controller->ering, 4096, TYPE_EVENT);

    // Set up event ring segment table.
    controller->erst.entries = (struct xhci_erst_entry *)malloc(4096);
    controller->erst.dma = (size_t)controller->erst.entries - KERNEL_START_MEMORY;
    controller->erst.num_segs = 1;
    controller->erst.entries->addr = controller->ering.trbs_dma;
    controller->erst.entries->size = controller->ering.size;
    controller->erst.entries->reserved = 0;

// We only need one interrupter since we either use msi
// or pin-based irqs.
// Set the event ring dequeue pointer
#define XHCI_ERDP_MASK (~(0xFUL))
    uint64_t erdp = controller->run_regs->irs[0].erdp & ~XHCI_ERDP_MASK;
    erdp = erdp | (controller->ering.trbs_dma & XHCI_ERDP_MASK);
    controller->run_regs->irs[0].erdp = erdp;
    // enable interrupts for this interrupter
    controller->run_regs->irs[0].iman = 1 << 1;

// Set up event ring size
#define XHCI_ERST_SIZE_MASK 0xFFFF
    uint32_t erstsz =
        controller->run_regs->irs[0].erstsz & ~XHCI_ERST_SIZE_MASK;
    erstsz = erstsz | controller->erst.num_segs;
    controller->run_regs->irs[0].erstsz = erstsz;

// Set the base address for the event ring segment table
#define XHCI_ERST_ADDR_MASK (~(0x3FUL))
    uint64_t erstba =
        controller->run_regs->irs[0].erstba & ~XHCI_ERST_ADDR_MASK;
    erstba = erstba | (controller->erst.dma & XHCI_ERST_ADDR_MASK);
    controller->run_regs->irs[0].erstba = erstba;

    // Start controller again
    cmd = controller->op_regs->usbcmd;
    cmd |= 0x1 | 1 << 2;
    controller->op_regs->usbcmd = cmd;

    //task_tcreate(0, tcreate_fn_call, \
                 tcreate_fn_call_data(0, &xhci_irq_handler, controller));

    // Wait for the controller to restart
     
    while ((controller->op_regs->usbsts & 0x1)) {
    };

    xhci_detect_devices(controller);
    
}

/**
 * @brief 
 * 
 * @return ** Driver* 
 */
Driver* XHCIController::GetDriver()
{
    return (Driver*)this;
}




  
 
/**
 * @brief 
 * 
 * @param controller 
 * @return ** void 
 * Take the controller's ownership from the BIOS
 */
void xhci_take_controller(struct xhci_hcd *controller) {
    uint32_t cparams;

    cparams = controller->cap_regs->hccparams1;

    size_t eoff = (((cparams & 0xFFFF0000) >> 16) * 4);
    volatile uint32_t *extcap =
        (uint32_t *)((size_t)controller->cap_regs + eoff);
    if (!extcap) {
        return;
    }

    // find the legacy capability
    while (1) {
        uint32_t val = *extcap;
        if (val == 0xFFFFFFFF) {
            break;
        }
        if (!(val & 0xFF)) {
            break;
        }

        DEBUG("usb/xhci found extcap: %X %X", val & 0xFF, extcap);
        if ((val & 0xff) == 1) {
            // Bios semaphore
            volatile uint8_t *bios_sem = (uint8_t *)((size_t)(extcap) + 0x2);
            if (*bios_sem) {
                DEBUG("usb/xhci: device is bios-owned");
                volatile uint8_t *os_sem = (uint8_t *)((size_t)(extcap) + 0x3);
                *os_sem = 1;
                while (1) {
                    bios_sem = (uint8_t *)((size_t)(extcap) + 0x2);
                    if (*bios_sem == 0) {
                        DEBUG("usb/xhci: device is no longer bios-owned");
                        break;
                    }
                  //  wait(100);
                }
            }
        }

        uint32_t *old = (uint32_t *)extcap;
        extcap = (uint32_t *)((size_t)extcap + (((val >> 8) & 0xFF) << 2));
        if (old == extcap) {
            break;
        }
    }
}

/**
 * @brief 
 * 
 * @param seg 
 * @param size 
 * @param type 
 * @return ** int 
 */
int xhci_setup_seg(struct xhci_seg *seg, uint32_t size, uint32_t type) {
    void *addr = malloc(size);
    seg->trbs = (union xhci_trb *)addr;
    seg->trbs_dma = (size_t)seg->trbs - KERNEL_START_MEMORY;
    struct xhci_link_trb *link;

    seg->size = size / 16;
    seg->next = NULL;
    seg->type = (xhci_seg_type)type;
    seg->cycle_state = 1;
    seg->enq = (uint64_t)seg->trbs;
    seg->deq = (uint64_t)seg->trbs;
    memset((void *)seg->trbs, 0, size);

    if (type != TYPE_EVENT) {
        link = (struct xhci_link_trb *)(seg->trbs + seg->size - 1);
        link->addr = seg->trbs_dma;
        link->field2 = 0;
        link->field3 = (0x1 | TRB_CMD_TYPE(TRB_LINK));
        *seg->seg_events = (struct xhci_event *)malloc(sizeof(struct xhci_event) * 4096);
    }
  //  seg->lock = new_lock;
    return 1;
}

/**
 * @brief 
 * 
 * @param controller 
 * @return ** void 
 */
void xhci_get_port_speeds(struct xhci_hcd *controller) {
    uint32_t cparams;
    cparams = controller->cap_regs->hccparams1;

    size_t eoff = (((cparams & 0xFFFF0000) >> 16) * 4);
    volatile uint32_t *extcap =
        (uint32_t *)((size_t)controller->cap_regs + eoff);
    if (!extcap) {
        return;
    }

    while (1) {
        uint32_t val = (uint32_t)*extcap;
        if (val == 0xFFFFFFFF) {
            break;
        }
        if (!(val & 0xFF)) {
            break;
        }

        DEBUG("usb/xhci found extcap: %X %X", val & 0xFF, extcap);
        if ((val & 0xff) == 2) {
            uint32_t *old_ext = (uint32_t *)extcap;

            struct xhci_port_protocol protocol = {0};
            uint32_t value = *extcap;
            protocol.major = (value >> 24) & 0xFF;
            protocol.minor = (value >> 16) & 0xFF;
            extcap++;
            value = *extcap;
            protocol.name[0] = (char)(value & 0xFF);
            protocol.name[1] = (char)((value >> 8) & 0xFF);
            protocol.name[2] = (char)((value >> 16) & 0xFF);
            protocol.name[3] = (char)((value >> 24) & 0xFF);
            protocol.name[4] = '\0';
            extcap++;
            value = *extcap;
            protocol.compatible_port_off = value & 0xFF;
            protocol.compatible_port_count = (value >> 8) & 0xFF;
            protocol.protocol_specific = (value >> 16) & 0xFF;
            int speed_id_cnt = (value >> 28) & 0xF;
            extcap++;
            value = *extcap;
            protocol.protocol_slot_type = value & 0xF;
            extcap++;

            for (int i = 0; i < speed_id_cnt; i++) {
                value = *extcap;

                protocol.speeds[i].value = value & 0xF;
                protocol.speeds[i].exponent = (value >> 4) & 0x3;
                protocol.speeds[i].type = (value >> 6) & 0x3;
                protocol.speeds[i].full_duplex = (value >> 8) & 0x1;
                protocol.speeds[i].link_protocol = (value >> 14) & 0x3;
                protocol.speeds[i].mantissa = (value >> 16) & 0xFFFF;

                extcap++;
            }

            color_kdebug(KPRN_INFO, "usb/xhci: port speed capability");
            color_kdebug(KPRN_INFO, 
                   "usb/xhci: protocol version: %u.%u, name: %s, offset: %X, "
                   "count = %X\n",
                   protocol.major, protocol.minor, protocol.name,
                   protocol.compatible_port_off,
                   protocol.compatible_port_count);

            extcap = old_ext;
            controller->protocols[controller->num_protcols] = protocol;
            controller->num_protcols++;
        }

        uint32_t *old = (uint32_t *)extcap;
        extcap = (uint32_t *)((size_t)extcap + (((val >> 8) & 0xFF) << 2));
        if (old == extcap) {
            break;
        }
    }
}

/**
 * @brief 
 * 
 * @param controller 
 * @return ** void 
 */
void xhci_detect_devices(struct xhci_hcd *controller) {
    xhci_get_port_speeds(controller);

    for (int i = 0; i < controller->num_protcols; i++) {
        for (int j = controller->protocols[i].compatible_port_off;
             (j - controller->protocols[i].compatible_port_off) <
             controller->protocols[i].compatible_port_count;
             j++) {
            volatile struct xhci_port_regs *port_regs1 =
                &controller->op_regs->prs[j - 1];
            // try to power the port if it's off
            if (!(port_regs1->portsc & (1 << 9))) {
                port_regs1->portsc = 1 << 9;
              wait_ms(20);
                if (!(port_regs1->portsc & (1 << 9))) {
                    // TODO use interrupt here
                    continue;
                }
            }

            // clear status change bits
            port_regs1->portsc = (1 << 9) | ((1 << 17) | (1 << 18) | (1 << 20) |
                                             (1 << 21) | (1 << 22));

            // usb 2 devices have bit 4 as reset, usb 3 has 31
            if (controller->protocols[i].major == 2) {
                port_regs1->portsc = (1 << 9) | (1 << 4);
            } else {
                port_regs1->portsc = (1 << 9) | (1 << 31);
            }

            int timeout = 0;
            int reset = 0;
            while (1) {
                if (port_regs1->portsc & (1 << 21)) {
                    reset = 1;
                    break;
                }
                if (timeout++ == 500) {
                    break;
                }
             wait_ms(1);
            }
            if (!reset) {
                continue;
            }
            // apparently this delay is necessary
           wait_ms(3);
            if (port_regs1->portsc & (1 << 1)) {
                port_regs1->portsc =
                    (1 << 9) |
                    ((1 << 17) | (1 << 18) | (1 << 20) | (1 << 21) | (1 << 22));
                xhci_init_device(controller, j - 1, controller->protocols[i]);
            }
        }
    }
}

/**
 * @brief 
 * 
 * @param controller 
 * @param port 
 * @param proto 
 * @return ** int 
 */
int xhci_init_device(struct xhci_hcd *controller, uint32_t port,
                     struct xhci_port_protocol proto) {
    xhci_enable_slot(controller);
    if (!controller->slot_id) {
        color_kdebug(KPRN_ERR, "usb/xhci: failed to get slot id.\n");
    }
    color_kdebug(KPRN_INFO, "usb/xhci: Initializing device, slot id: id: %x port: %x\n",
           controller->slot_id, port);
    uint32_t slot_id = controller->slot_id;

    struct xhci_dev *xhci_dev = (struct xhci_dev *)malloc(sizeof(xhci_dev));
    struct xhci_slot_ctx *slot;
    struct xhci_control_ctx *ctrl;
    struct xhci_ep_ctx *ep0;
    uint16_t max_packet = 0;

    xhci_setup_context(&xhci_dev->in_ctx, 4096, XHCI_CTX_TYPE_INPUT);
    ctrl = xhci_get_control_ctx(&xhci_dev->in_ctx);
    ctrl->add_flags = (0x3);
    ctrl->drop_flags = 0;

    slot = xhci_get_slot_ctx(&xhci_dev->in_ctx, controller->context_size);

    // TODO hub support

    slot->field1 = (1 << 27) | ((0x4 << 10) << 10);
    slot->field2 = ROOT_HUB_PORT(port + 1);

    ep0 = xhci_get_ep0_ctx(&xhci_dev->in_ctx, controller->context_size);

    volatile struct xhci_port_regs *port_regs = &controller->op_regs->prs[port];
    uint8_t port_speed = (port_regs->portsc >> 10) & 0b1111;
    switch (port_speed) {
        case 1:
            // TODO handle full speed
            color_kdebug(KPRN_ERR, "usb/xhci: full speed devices are not handled\n");
            // TODO set a temporary max transfer size and read the first 8 bytes
            // of the device descriptor
            return -1;
        case 2:
            max_packet = 8;
            break;
        case 3:
            max_packet = 64;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            max_packet = 512;
            break;
    }

    uint32_t val = EP_TYPE(EP_CTRL) | MAX_BURST(0) | ERROR_COUNT(3) |
                   MAX_PACKET_SIZE(max_packet);
    ep0->field2 = val;
    // Average trb length is always 8.
    ep0->field4 = 8;
    xhci_setup_seg(&xhci_dev->control, 4096, TYPE_CTRL);
    ep0->deq_addr =
        (xhci_dev->control.trbs_dma | xhci_dev->control.cycle_state);

    xhci_setup_context(&xhci_dev->out_ctx, 4096, XHCI_CTX_TYPE_DEVICE);
    controller->dcbaap[slot_id] = xhci_dev->out_ctx.dma_addr;
    slot = xhci_get_slot_ctx(&xhci_dev->out_ctx, controller->context_size);
    xhci_send_addr_device(controller, slot_id, xhci_dev->in_ctx.dma_addr, 0);
    ctrl->add_flags = (0x1);
    xhci_configure_ep(controller, slot_id, xhci_dev->in_ctx.dma_addr);

    struct usb_dev_t udevice = {0};
    udevice.max_packet_size = 512;
    udevice.controller = xhci_controller;
    xhci_dev->xhci_controller = controller;
    udevice.internal_controller = controller;
    controller->xdevs[slot_id] = xhci_dev;
    xhci_dev->slot_id = slot_id;

   // if (usb_add_device(udevice, slot_id))
     //   return -1;
    return 0;
}

/**
 * @brief 
 * 
 * @param controller 
 * @return ** int 
 */
int xhci_enable_slot(struct xhci_hcd *controller) {
    uint32_t field1 = 0, field2 = 0, field3 = 0,
             field4 = TRB_CMD_TYPE(TRB_ENABLE_SLOT);
    struct xhci_event ev =  {0};
    xhci_send_command(controller, field1, field2, field3, field4, &ev);
   // event_await(&ev.event);
   wait_ms(10010);
    if (TRB_STATUS(ev.trb.status)) {
        int slot = TRB_SLOT_ID(ev.trb.flags);
        controller->slot_id = slot;
        return 0;
    } else {
        color_kdebug(KPRN_ERR, "usb/xhci: failed to enable slot\n");
        controller->slot_id = 0;
        return 1;
    }
}

/**
 * @brief 
 * 
 * @param controller 
 * @param field1 
 * @param field2 
 * @param field3 
 * @param field4 
 * @param ev 
 * @return ** int 
 */
int xhci_send_command(struct xhci_hcd *controller, uint32_t field1,
                             uint32_t field2, uint32_t field3, uint32_t field4,
                             struct xhci_event *ev) {

    volatile struct xhci_db_regs *dbr;

    dbr = controller->db_regs;

    uint32_t index =
        (controller->crseg.enq - (uint64_t)controller->crseg.trbs) / 0x10;
    struct xhci_command_trb *command = (struct xhci_command_trb *)xhci_get_trb(&controller->crseg);
    fill_trb_buf(command, field1, field2, field3, field4, 0);
    controller->crseg.seg_events[index] = ev;

    dbr->db[0] = 0;

    return 0;
} 

/**
 * @brief 
 * 
 * @param seg 
 * @return ** void* 
 */
void *xhci_get_trb(struct xhci_seg *seg) {
    uint64_t enq;
    uint32_t index;
    struct xhci_link_trb *link;

    enq = seg->enq;
    index = (enq - (uint64_t)seg->trbs) / 16 + 1;
    // Check if we should cycle back to the beginning
    if (index == (seg->size - 1)) {
        seg->enq = (uint64_t)seg->trbs;
        seg->cycle_state ^= seg->cycle_state;
        link = (struct xhci_link_trb *)(seg->trbs + seg->size - 1);
        link->addr = (seg->trbs_dma);
        link->field2 = 0;
        link->field3 = (0x1 | TRB_CMD_TYPE(TRB_LINK));
    } else {
        seg->enq = seg->enq + 16;
    }
    return (void *)enq;
}

/**
 * @brief 
 * 
 * @param cmd 
 * @param field1 
 * @param field2 
 * @param field3 
 * @param field4 
 * @param first 
 * @return ** void 
 */
void fill_trb_buf(struct xhci_command_trb *cmd, uint32_t field1,
                         uint32_t field2, uint32_t field3, uint32_t field4,
                         int first) {
    uint32_t val, cycle_state;

    cmd->field[0] = (field1);
    cmd->field[1] = (field2);
    cmd->field[2] = (field3);

    if (!first) {
        val = (cmd->field[3]);
        cycle_state = (val & 0x1) ? 0 : 1;
        val = cycle_state | (field4 & ~0x1);
        cmd->field[3] = (val);
    } else {
        cmd->field[3] = field4;
    }

    return;
}

/**
 * @brief 
 * 
 * @param ctx 
 * @param size 
 * @param type 
 * @return ** void 
 */
void xhci_setup_context(struct xhci_ctx *ctx, uint32_t size, uint32_t type) {
    void *addr = malloc(size);
    ctx->addr = (uint8_t *)addr;
    ctx->dma_addr = (size_t)ctx->addr - KERNEL_START_MEMORY;
    ctx->type = type;
    ctx->size = size;
}

/**
 * @brief 
 * 
 * @param ctx 
 * @return ** struct xhci_control_ctx* 
 */
struct xhci_control_ctx *xhci_get_control_ctx(struct xhci_ctx *ctx) {
    if (ctx->type == XHCI_CTX_TYPE_INPUT)
        return (struct xhci_control_ctx *)ctx->addr;
    return NULL;
}

/**
 * @brief 
 * 
 * @param ctx 
 * @param ctx_size 
 * @return ** struct xhci_ep_ctx* 
 */
struct xhci_ep_ctx *xhci_get_ep0_ctx(struct xhci_ctx *ctx,
                                            uint32_t ctx_size) {
    uint32_t offset = ctx_size;
    int mul = 1;

    if (ctx->type == XHCI_CTX_TYPE_INPUT) {
        mul++;
    }
    return (struct xhci_ep_ctx *)(ctx->addr + offset * mul);
}

/**
 * @brief 
 * 
 * @param ctx 
 * @param ctx_size 
 * @return ** struct xhci_slot_ctx* 
 */
struct xhci_slot_ctx *xhci_get_slot_ctx(struct xhci_ctx *ctx,
                                               uint32_t ctx_size) {
    uint32_t offset = 0;

    if (ctx->type == XHCI_CTX_TYPE_INPUT)
        offset += ctx_size;
    return (struct xhci_slot_ctx *)((size_t)ctx->addr + offset);
}

/**
 * @brief 
 * 
 * @param xhcd 
 * @param slot_id 
 * @param dma_in_ctx 
 * @param bsr 
 * @return ** int 
 */
int xhci_send_addr_device(struct xhci_hcd *xhcd, uint32_t slot_id,
                                 uint64_t dma_in_ctx, int bsr) {
    uint32_t field1, field2, field3, field4;

    field1 = TRB_ADDR_LOW(dma_in_ctx) & ~0xF;
    field2 = TRB_ADDR_HIGH(dma_in_ctx);
    field3 = 0;
    field4 =
        TRB_CMD_TYPE(TRB_ADDRESS_DEV) | TRB_CMD_SLOT_ID(slot_id) | (bsr << 9);
    struct xhci_event ev = {0};
    xhci_send_command(xhcd, field1, field2, field3, field4, &ev);
   // event_await(&ev.event);
     wait_ms(100);
    if ((ev.trb.status >> 24) != 1) {
        color_kdebug(KPRN_ERR,
               "usb/xhci: Error while attempting to address device: %X\n",
               ev.trb.status);
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief 
 * 
 * @param xhcd 
 * @param slot_id 
 * @param dma_in_ctx 
 * @return ** int 
 */
int xhci_configure_ep(struct xhci_hcd *xhcd, uint32_t slot_id,
                             uint64_t dma_in_ctx) {
    uint32_t field1, field2, field3, field4;

    field1 = TRB_ADDR_LOW(dma_in_ctx) & ~0xF;
    field2 = TRB_ADDR_HIGH(dma_in_ctx);
    field3 = 0;
    field4 = TRB_CMD_TYPE(TRB_CONFIG_EP) | TRB_CMD_SLOT_ID(slot_id);
    struct xhci_event ev = {0};
    xhci_send_command(xhcd, field1, field2, field3, field4, &ev);
   // event_await(&ev.event);
    if ((ev.trb.status >> 24) != 1) {
        color_kdebug(KPRN_ERR,
               "usb/xhci: Error while attempting to configure endpoint\n");
        return 1;
    } else {
        return 0;
    }
}


   
    