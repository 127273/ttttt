
#include <drivers/net/etherframe.h>
#include <kernel.h>

using namespace myos;
using namespace myos::net;
using namespace myos::drivers;

EtherFrameHandler::EtherFrameHandler(EtherFrameProvider *backend, uint16_t etherType)
{
    this->etherType_BE = ((etherType & 0x00FF) << 8) | ((etherType & 0xFF00) >> 8);
    this->backend = backend;
    backend->handlers[etherType_BE] = this;
}

EtherFrameHandler::~EtherFrameHandler()
{
    if (backend->handlers[etherType_BE] == this)
        backend->handlers[etherType_BE] = 0;
}

bool EtherFrameHandler::OnEtherFrameReceived(uint8_t *etherframePayload, uint32_t size)
{
    UNUSED(etherframePayload);
    UNUSED(size);
    return false;
}

void EtherFrameHandler::Send(uint64_t dstMAC_BE, uint8_t *data, uint32_t size)
{
    backend->Send(dstMAC_BE, etherType_BE, data, size);
}

uint32_t EtherFrameHandler::GetIPAddress()
{
    return backend->GetIPAddress();
}

EtherFrameProvider::EtherFrameProvider(myos::drivers::NetDataHandlerBaseClass *backend)

{
    DEBUG("init");
    //this = (EtherFrameProvider*)malloc(sizeof(EtherFrameProvider));
    for (uint32_t i = 0; i < MAX_PORTS; i++)
    {
        //handlers[i] = (EtherFrameHandler *)malloc(sizeof(EtherFrameHandler));
        handlers[i] = NULL;
    }
    DEBUG("init");
    this->backend = backend;
  
    // backend = (NetDataHandlerBaseClass*)malloc(sizeof(NetDataHandlerBaseClass));
}

EtherFrameProvider::~EtherFrameProvider()
{
}

bool EtherFrameProvider::OnRawDataReceived(uint8_t *buffer, uint32_t size)
{
    if (size < sizeof(EtherFrameHeader))
        return false;

    //printk("frame onrerrdettet");
    EtherFrameHeader *frame = (EtherFrameHeader *)buffer;
    bool sendBack = false;

    /* need change arp请求或者其他请求 */
    if (frame->dstMAC_BE == 0xFFFFFFFFFFFF || frame->dstMAC_BE == backend->GetMACAddress() )
    {
        if (handlers[frame->etherType_BE] != 0)
            sendBack = handlers[frame->etherType_BE]->OnEtherFrameReceived(
                buffer + sizeof(EtherFrameHeader), size - sizeof(EtherFrameHeader));
    }

    if (sendBack)
    {
        frame->dstMAC_BE = frame->srcMAC_BE;
        frame->srcMAC_BE = backend->GetMACAddress();
    }

    return sendBack;
}

void EtherFrameProvider::Send(uint64_t dstMAC_BE, uint16_t etherType_BE, uint8_t *buffer, uint32_t size)
{
    uint8_t *buffer2 = (uint8_t *)malloc(sizeof(EtherFrameHeader) + size);
    EtherFrameHeader *frame = (EtherFrameHeader *)buffer2;

    frame->dstMAC_BE = dstMAC_BE;
    DEBUG("dst MAC %x\n", dstMAC_BE);

    if (backend != 0)
        frame->srcMAC_BE = backend->GetMACAddress();

    frame->etherType_BE = etherType_BE;

    uint8_t *src = buffer;
    uint8_t *dst = buffer2 + sizeof(EtherFrameHeader);
    for (uint32_t i = 0; i < size; i++)
    {
        dst[i] = src[i];
    }
    //printk("%s",buffer2);
    /* ??? 这里传的是buffer2 而不是buffer, 或者是dst? 传的是buffer 则会发生问题 */

    //DEBUG("%x", buffer2[0]);
    if (backend != 0)
        backend->Send(buffer2, size + sizeof(EtherFrameHeader));
   free(buffer2);
}

uint32_t EtherFrameProvider::GetIPAddress()
{
    if (backend != 0)
        return backend->GetIPAddress();
    return 0;
}

uint64_t EtherFrameProvider::GetMACAddress()
{
    if (backend != 0)
        return backend->GetMACAddress();
    return 0;
}
