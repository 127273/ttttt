//include <multiboot.h>
#include <kernel.h>

#include <stdint.h>
#include <string.h>

#include <memory.h>
#include <elf.h>
#include <x86.h>
#include <stdarg.h>
#include <_printf.h>
#include <isr.h>
#include <timer.h>
#include <keyboard.h>
#include <hpet.h>
#include <process.h>
#include <serial.h>
#include <pic.h>
#include <mmu_paging.h>
#include <mmu_heap.h>
#include <mouse.h>
#include <assert.h>
#include <vesa.h>
#include <window_manager.h>
#include <pci.h>
#include <ata.h>
#include <fat.h>
#include <stdio.h>
#include <multiboot_head.h>
#include <pci.h>
#include <apic.h>
#include <acpi.h>
#include <smp.h>
#include <mem.h>
#include <drivers/usb/xchi.h>

#include <drivers/net/net.h>
#include <drivers/net/arp.h>
#include <drivers/net/etherframe.h>
#include <drivers/net/i8254/i8254.h>

#include <driver.h>

#include <64bit.h>
#include <smpdefines.inc>
using namespace myos;
using namespace grub;
using namespace Graphical;
using namespace drivers;
using namespace hardware;
using namespace PCI;
using namespace net;
using namespace APIC;


#define GDT_NULL        0x00
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28 


int serial_printf_help(unsigned c, void *ptr __UNUSED__) {
  serial_write_com(1, c);
  return 0;
}

void HALT_AND_CATCH_FIRE(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
  x86_hlt(); 
  while(1) {}
}

void _kdebug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
}

/**
 * @brief 
 * 
 * @param colorFont 
 * @param fmt 
 * @param ... 
 * @return ** void 
 * 
 * this function is very fun and fun
 * 
 * char Array is a big difference with char*
 * The Ayyay is the best choice whenever
 */
void color_kdebug(int colorFont, const char *fmt, ...) 
{
  static char kdebugBuffer[512];
  static int kdebugLen = 0, cursor = 0;
  static char *fontColorArray[] = {"\e[36minfo: ",  "\e[33mwarning: ", \
                  "\e[31mERROR: ", "\e[36mDEBUG: ", "\e[31mPANIC: " };
  char* fontColorPrefix = NULL, *fontColorSuffix = "\e[37m";
  switch (colorFont) {
        case KPRN_INFO: 
        case KPRN_WARN:
        case KPRN_ERR:
        case KPRN_PANIC:
          fontColorPrefix = fontColorArray[colorFont];
          break;

        default:
           case KPRN_DBG:
            fontColorPrefix = fontColorArray[3];
    }
  
  while(cursor < strlen(fontColorPrefix)) kdebugBuffer[kdebugLen++] = *fontColorPrefix++;
 
  cursor = 0;
  while(cursor < strlen(fmt)) kdebugBuffer[kdebugLen++] = *fmt++;
  cursor = 0;
  while(cursor < strlen(fontColorSuffix)) kdebugBuffer[kdebugLen++] = *fontColorSuffix++;

  kdebugBuffer[kdebugLen] = '\0';
 
  
  fmt = kdebugBuffer;
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);

  while( kdebugLen >= 0)kdebugBuffer[kdebugLen--] = '\0';
  kdebugLen = 0;
 
}

void idle() {
  while(1) {
    x86_hlt();
  }
}

extern "C" void slave_pro()
{
  DEBUG("this is slave\n");
  while(1);
}


void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)KERNEL_USER_VIEW_VMA;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

//extern "C" uint8_t _APU_boot_start[];
//extern "C" uint8_t _APU_boot_end[];
extern  "C" uint64_t  boot_start;
extern  "C" uint64_t  boot_end;
extern  "C" uint64_t trampoline_start;
extern  "C" uint64_t trampoline_end;
extern "C" uint64_t trampoline32;
extern "C" void kmain(unsigned long magic __UNUSED__, multiboot_info_t* mbi_phys) {
  
  init_kernel_serial();
  init_kernel_isr();
  GrubMessage GrubInformation(TO_VMA_PTR(multiboot_info_t *, mbi_phys));  
  
  Vesa GraVesa(GrubInformation.GetFrameBuffer());


  init_kernel_paging(GrubInformation.GetRAM());
 
  init_acpi();  

  APIC::Initialize();
  //*((uint16_t*)0x1000) = 0;
  //SMP::Initialize();
  GetCpuSpeed();
  DEBUG("Thermal %x \n",  x86_rdmsr(0x1a3) );
 
//   *(uint8_t*)TO_VMA_U64(0x40000) = 0xf4;
  
//   //memcpy((unsigned char *)0xffff800000020000, _APU_boot_start,(unsigned long)&_APU_boot_end - (unsigned long)&_APU_boot_start);

  InterruptManager GlobalInterrupts;
  DriverManager drvManager;
  UsefulDevices UFdevice;
 
  PeripheralComponentInterconnectController PCIController(&UFdevice, &GlobalInterrupts);
 
  //PCIController.SelectDrivers(&drvManager);
 // init_kernel_pic();
//
  // asm volatile ("sti":::"memory");
  //     memcpy((uint8_t *)TO_VMA_U64(0x20000), &boot_start, (uint64_t)&boot_end - (uint64_t)&boot_start);
 // //  memcpy((uint8_t *)TO_VMA_U64(0x20000), &trampoline_start, (uint64_t)&trampoline_end - (uint64_t)&trampoline_start);
     //memcpy((uint8_t *)TO_VMA_U64(0x30000), (void*)&slave_pro,0x30);
      //*(uint8_t*)TO_VMA_U64(0x20100) = 0xf4;
      //*(uint8_t*)(TO_VMA_U64(trampoline32 - boot_start + 0x20000)) = 0xf4;
       //DEBUG("dd %x",  mem_inb(TO_VMA_U64(0xfd412 + 0x80)));
  //   send_ipi(1,0x500);
  //    send_ipi( 1,0x620); 
  //     asm volatile ("cli":::"memory");
  // while(1);
//mem_inb(0x70);
 /** 把键盘放在鼠标前面就会出错 ?  现在没问题了*/
  KeyboardDriver keyboard(&GlobalInterrupts, 0x21);
  drvManager.AddDriver(&keyboard, "keyboard");


  MouseDriver mouse(&GlobalInterrupts, 0x2c);
  drvManager.AddDriver(&mouse, "mouse");

 init_kernel_timer();
  Aliceos::drivers::HEPTTimerControl hh(&GlobalInterrupts, 0x22);
  drvManager.AddDriver(&hh, "Timer");
   
  //init_kernel_mouse();

  

  
 // init_kernel_keyboard();
  init_kernel_ata();
  init_kernel_fat();
  //keyboard.Activate();
  
  Intel_82545 intelnet(UFdevice.driverGetBaseAddress(0x8086, 0x100f), \
         UFdevice.GetInterrupt(0x8086, 0x100f), &GlobalInterrupts);


  if(drvManager.AddDriver(intelnet.GetDriver(), "IntelNet") == false)
      DEBUG("add net fail");
  
  //XHCIController echi(UFdevice.driverGetBaseAddress(0x1033, 0x194), \
         UFdevice.GetInterrupt(0x1033, 0x194), &GlobalInterrupts);
  //if(drvManager.AddDriver(echi.GetDriver(), "InteaalNet") == false)
    DEBUG("usb add fail\n");
  //drvManager.activateDriver(0x8086, 0x100f); 

  drvManager.ActivateAll();
  GlobalInterrupts.enable();
 while(1);

  //SMP::Initialize();
 
  NetDataHandlerBaseClass *nett = &intelnet;
  uint8_t ip1 = 192, ip2 = 168, ip3 = 13, ip4 = 15;
  uint32_t ip_be = ((uint32_t)ip4 << 24) | ((uint32_t)ip3 << 16) | ((uint32_t)ip2 << 8) | (uint32_t)ip1;
 
  nett->SetIPAddress(ip_be);

  EtherFrameProvider etherframe(nett);

  AddressResolutionProtocol arp(&etherframe);
  
  // IP Address of the default gateway
 // uint8_t gip1 = 10, gip2 = 0, gip3 = 1, gip4 = 1;
 uint8_t gip1 = 192, gip2 = 168, gip3 = 13, gip4 = 2;
  uint32_t gip_be = ((uint32_t)gip4 << 24) | ((uint32_t)gip3 << 16) | ((uint32_t)gip2 << 8) | (uint32_t)gip1;


  uint8_t subnet1 = 255, subnet2 = 255, subnet3 = 255, subnet4 = 0;
  uint32_t subnet_be = ((uint32_t)subnet4 << 24) | ((uint32_t)subnet3 << 16) | ((uint32_t)subnet2 << 8) | (uint32_t)subnet1;

  arp.BroadcastMACAddress(gip_be);
  //printf("This  ");
 //while(1);

  //  MemHandlerBaseClass zz(0);
  //zz.inl((0xffff8000fee00030));
  //DEBUG("usb 0x%x \n", mem_inw(TO_VMA_U64(0xfebb5000 + 0x08)));
  init_kernel_window_manager();
  
  create_kernel_process((void *)idle);  
  create_user_process_file("/apps/desktop/desktop");
DEBUG("jmp\n");
  do_first_task_jump();
  while (1)
  { DEBUG("ss");  schedule();}
}
