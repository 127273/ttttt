
#include <drivers/net/arp.h>
#include <mmu_heap.h>
#include <kernel.h>
using namespace myos;
using namespace myos::net;
using namespace myos::drivers;

#define SendArpNum 0x3 /* 默认一次性发送3个arp消息 */


AddressResolutionProtocol::AddressResolutionProtocol(EtherFrameProvider *backend)
    : EtherFrameHandler(backend, 0x806)
{
    numCacheEntries = 0; //1
    IPcache[0] = 0x020da8c0;
    MACcache[0] = 0x005056e33d78;
    sendnum = SendArpNum;
}

AddressResolutionProtocol::~AddressResolutionProtocol()
{
}

bool AddressResolutionProtocol::OnEtherFrameReceived(uint8_t *etherframePayload, uint32_t size)
{
    if (size < sizeof(AddressResolutionProtocolMessage))
        return false;

    AddressResolutionProtocolMessage *arp = (AddressResolutionProtocolMessage *)etherframePayload;
    if (arp->hardwareType == 0x0100)
    {

        if (arp->protocol == 0x0008 && arp->hardwareAddressSize == 6 && arp->protocolAddressSize == 4 && arp->dstIP == backend->GetIPAddress())
        {

            switch (arp->command)
            {

            case 0x0100: // request
                arp->command = 0x0200;
                arp->dstIP = arp->srcIP;
                arp->dstMAC = arp->srcMAC;
                arp->srcIP = backend->GetIPAddress();
                arp->srcMAC = backend->GetMACAddress();
                return true;
                break;

            case 0x0200: // response
                if (numCacheEntries < MaxNumCacheEntries)
                {
                    if (isMACInThis(arp->srcMAC))
                        return false;
                    IPcache[numCacheEntries] = arp->srcIP;
                    MACcache[numCacheEntries] = arp->srcMAC;
                    numCacheEntries++;
                    //printk("\n    srcadnsndddddddddddddddddddddddddddd");
                }
                break;
            }
        }
    }

    return false;
}

void AddressResolutionProtocol::BroadcastMACAddress(uint32_t IP_BE)
{
    AddressResolutionProtocolMessage arp;
    arp.hardwareType = 0x0100;   // ethernet
    arp.protocol = 0x0008;       // ipv4
    arp.hardwareAddressSize = 6; // mac
    arp.protocolAddressSize = 4; // ipv4
    arp.command = 0x0200;        // "response"

    arp.srcMAC = backend->GetMACAddress();
    // printk(arp.srcMAC);
    arp.srcIP = backend->GetIPAddress();
    arp.dstMAC = Resolve(IP_BE);
    arp.dstIP = IP_BE;

    /* 将自己的地址告诉网管 */
    /* 传的是上面这个arp结构体的数据*/
    this->Send(arp.dstMAC, (uint8_t *)(&arp), sizeof(AddressResolutionProtocolMessage));
}

bool AddressResolutionProtocol::isMACInThis(uint64_t MAC)
{
    for (int i = 0; i < numCacheEntries; ++i)
    {
        if (MACcache[i] == MAC)
            return true;
    }
    return false;
}

void AddressResolutionProtocol::RequestMACAddress(uint32_t IP_BE)
{

    AddressResolutionProtocolMessage arp;
    arp.hardwareType = 0x0100;   // ethernet
    arp.protocol = 0x0008;       // ipv4
    arp.hardwareAddressSize = 6; // mac
    arp.protocolAddressSize = 4; // ipv4
    arp.command = 0x0100;        // request

    arp.srcMAC = backend->GetMACAddress();
    arp.srcIP = backend->GetIPAddress();
    arp.dstMAC = 0xFFFFFFFFFFFF; // broadcast
    arp.dstIP = IP_BE;

    this->Send(arp.dstMAC, (uint8_t *)&arp, sizeof(AddressResolutionProtocolMessage));
}

uint64_t AddressResolutionProtocol::GetMACFromCache(uint32_t IP_BE)
{
    for (int i = 0; i < numCacheEntries; i++)
        if (IPcache[i] == IP_BE)
            return MACcache[i];
    return 0xFFFFFFFFFFFF; // broadcast address
}

uint64_t AddressResolutionProtocol::Resolve(uint32_t IP_BE)
{
    uint64_t result = GetMACFromCache(IP_BE);
    if (result == 0xFFFFFFFFFFFF)
        RequestMACAddress(IP_BE);

    int y = 3, x = 0;
    while (result == 0xFFFFFFFFFFFF) // possible infinite loop
    {
        if(y-- < 0)
            break;
        RequestMACAddress(IP_BE);
        if (GetMACFromCache(IP_BE) != 0xFFFFFFFFFFFF)
            break;
        DEBUG(" x:%x ", x++);
        sendnum--;
    }

    sendnum = SendArpNum;
    result = GetMACFromCache(IP_BE);
    DEBUG("over");
 
    return result;
}
