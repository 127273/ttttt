
#include <sound/intelHD.h>
#include <memory.h>
#include <kernel.h>
#include <mmu_heap.h>

using namespace myos;
using namespace myos::drivers;
using namespace myos::hardware;

bool intelHDHandler::SoundUp()
{
}

bool intelHDHandler::SoundDown()
{
}

bool intelHDHandler::SoundOpen()
{
}

bool intelHDHandler::SoundStop()
{
}

intelHDHandler::~intelHDHandler()
{
}

bool IntelHDSound::Reset()
{
    //corb_buffer = 0xDEADBE80;
    //  corb_buffer = (uint8_t*)MemoryManager::activeMemoryManager->malloc(1024 + 128);
    //
    return true;
}

IntelHDSound::IntelHDSound(PeripheralComponentInterconnectDeviceDescriptor *dev,
                           myos::hardware::InterruptManager *interrupts)
    : Driver(),
      InterruptHandler(interrupts, dev->interrupt + 0x20),
      memhandler(dev->mmap)
{
    SoundHandler = 0;
    mmio_address = dev->mmap;

    DEBUG("dev, %d", uint8_t(dev->interrupt + 0x20));
    if (Reset() == false)
    {
        DEBUG("intelHD reset fail \n");
        return;
    }

    //  memhandler.outb(0x48, 1); //number of commands to corb
    uint32_t *p = (uint32_t *)corb_buffer;
    //p[0] = (0xf00 << 8  | 0);
    DEBUG("dd %d  ", rirb_buffer);
    //write commands into CORB

    //memhandler.outb(0x48, 10);
    // memhandler.outb(0x5C, 3);
    //  mem_write8(mmio_address + 0x48, (uint8_t)10);

    /*	memhandler.outl(0x40, (uint32_t)corb_buffer);  //CORB buffer
	memhandler.outl(0x50, (uint32_t)rirb_buffer);  //RIRB buffer
	memhandler.outl(0x44, 0x00000000);  //CORB buffer
	memhandler.outl(0x54, 0x00000000);  //RIRB buffer

	//write commands into CORB

	(memhandler.inw(0x4A));

	memhandler.outb(0x48, 0); //number of commands to corb

	memhandler.inw(0x4A);
	memhandler.outw(0x4A, 0x8000); //reset corb
	memhandler.inw(0x4A);
	while( (memhandler.inw(0x4A) & 0x8000) != 0x8000) {  }  //wait

	memhandler.outw(0x4A, 0x0000); //reset corb
	memhandler.inw(0x4A);
	while( (memhandler.inw(0x4A) & 0x8000) != 0x0000) {  }  //wait

	memhandler.outw(0x58, 0x8000);

    while(memhandler.inw(0x58)  != 0) {   }

	memhandler.outb(0x5C, (memhandler.inb(0x5C) | 1));
	memhandler.outb(0x5C, (memhandler.inb(0x5C) & 250));
	memhandler.outb(0x4C, (memhandler.inb(0x4C) | 2));
	memhandler.outb(0x4C, (memhandler.inb(0x4C) & 0xFE));
    memhandler.inb(0x4C );
    for(int i=0; i<128; i++) {
          p[i] = (0xf00 << 8  | 0);
//( (cad << 28) | (nid << 20) | (verb << 8) | (command) )
    //	corb_write_command(0, i, 0xF00, 0, corb_buffer);
    }
    
    DEBUG(" ok");
	while(memhandler.inb(0x58)==0) {  } //here is forever cycle

*/
    DEBUG("codec %d  ", memhandler.inw(14));
    memhandler.outw(14, 1);

    DEBUG("codec %d  ", memhandler.inw(14));
    for (int i = 0; i < 6; i++)
    {
        p[i] = ((0 << 28) | (0xf00 << 8) | (0 << 20));
        //( (cad << 28) | (nid << 20) | (verb << 8) | (command) )
        //	corb_write_command(0, i, 0xF00, 0, corb_buffer);
    }

    memhandler.outb(0x48, 5);
    //memhandler.outb(0x5c,  3 );
    // memhandler.outb(0x4c, 3);
    //Sleep(2);
    //不启动rirb run  corb读写指针就会一直 ???
    //while(memhandler.inb(0x4a)!=5) {  DEBUG("%d,", memhandler.inb(0x4a));} //here is forever cycle

    DEBUG("aa: %d", memhandler.inb(0x4a));
    //memhandler.outb(0x4c,  0 );

    DEBUG("\nRIRB  ");
    DEBUG("aa: %d  ", memhandler.inb(0x58));
    uint32_t *p1 = (uint32_t *)(rirb_buffer);
    for (int i = 0; i < 20; i++)
    {
        DEBUG("%x ", p1[i]);
    }

    memhandler.outl(0x20, (1 << 30) | (1 << 30));
    //while(1);
    /* DEBUG("%x", corb_buffer[2]);
    //  mem_outl(corb_buffer, 0xF00);


    //memhandler.outb(0x4C,  2 );
    DEBUG("ssss%x", memhandler.inw(0x5e));

    DEBUG(" %x", memhandler.inw(0x48));
    
   //  memhandler.outb(0x5C,  1 );
    memhandler.outb(0x48, 1);
 
    DEBUG(" %x", memhandler.inw(0x48));
    DEBUG(" %x", memhandler.inw(0x4a));
    /* 
    DEBUG(" %x", memhandler.inw(0x0));
    DEBUG(" %x", memhandler.inw(0x04));
    DEBUG(" %x", memhandler.inw(0x18));

    DEBUG(" %x", memhandler.inw(0x84));
    DEBUG(" %x", p[0]);
   
   memhandler.outw(0x08, memhandler.inw(0x08) | 1 << 8);
  

   DEBUG("aaa: %x", memhandler.inb(0x58));
  
    DEBUG("uuuu %d  %x %x", i, rirb_buffer[1], rirb_buffer[2]);*/
    // while(1);
}

IntelHDSound::~IntelHDSound()
{
}

void IntelHDSound::Activate()
{
}

void IntelHDSound::SetHandler(SoundHandlerBaseClass *handler)
{
    SoundHandler = handler;
}
