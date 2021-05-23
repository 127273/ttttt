#include <cpu.h>
#include <stdint.h>
cpuid_info_t CPUID() {
	cpuid_info_t info;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;

	asm volatile("cpuid": "=b"(ebx),"=d"(edx),"=c"(ecx) : "a"(0)); // Get vendor string
	for (int i = 0; i < 4; i++) info.vendorString[i] = ebx >> (i * 8) & 0xFF; // Copy string to buffer
	for (int i = 0; i < 4; i++) info.vendorString[i+4] = edx >> (i * 8) & 0xFF;
	for (int i = 0; i < 4; i++) info.vendorString[i+8] = ecx >> (i * 8) & 0xFF;

	asm volatile("cpuid": "=d"(edx), "=c"(ecx) : "a"(1)); // Get features
	info.features_ecx = ecx;
	info.features_edx = edx;
	return info;
}

#include <kernel.h>
#include <string.h>
/** 因为我们从32位进入到64位模式 所以BIOS是属于32-bit entry point */
void GetCpuSpeed(){
   char* mem = (char*)(TO_VMA_U64(0xf0000));
   while((uint64_t)mem < TO_VMA_U64(0x100000))
   {
     if(!strncmp(mem, "_SM_", 4))
     {
       DEBUG("SMBIOS ok");
       break;
     }
     mem += 16;
   }
   if((uint64_t)mem >= TO_VMA_U64(0x100000))
   {
      DEBUG("SMBIOS is not ok");
      return;
   }
   SMBIOSEntryPoint *entry = (SMBIOSEntryPoint*)mem;

   int x = 0;
   uint32_t tableLength = entry->TableLength;
   uint64_t tableAddress = TO_VMA_U64(entry->TableAddress);
   DEBUG("%d %d\n", entry->TableLength, entry->NumberOfStructures);
   while(x < tableLength)
   {
    SMBIOSHeader* hh = (SMBIOSHeader*)(tableAddress);
    if(hh->Length == 0)
      break;
    switch (hh->Type)
    {
    case BIOSInformation:
       // DEBUG("length: %d  \t", hh->Length);
      /* code */
      break;
    case ProcessorInformation:
      DEBUG("processor\n");
    default:
      break;
    }
   DEBUG("length: %d  %d \t", hh->Length, hh->Type);
  
    x += hh->Length;
    tableAddress += hh->Length;
   }
}