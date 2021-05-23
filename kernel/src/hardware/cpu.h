#ifndef __CPU_H
#define __CPU_H
#include <stdint.h>
#include <system.h>
#include <tss.h>
static inline void get_cpuid(unsigned int Mop,unsigned int Sop,unsigned int * a,unsigned int * b,unsigned int * c,unsigned int * d)
{
	__asm__ __volatile__	(	"cpuid	\n\t"
					:"=a"(*a),"=b"(*b),"=c"(*c),"=d"(*d)
					:"0"(Mop),"2"(Sop)
				);
}

struct process;
template<typename T>
class FastList;

typedef struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

struct CPU{
	CPU* self;
    uint64_t id; // APIC/CPU id
    void* gdt; // GDT
	gdt_ptr_t gdtPtr;
//	Thread* currentThread = nullptr;
	process* idleProcess = nullptr;
	volatile int runQueueLock = 0;
//	FastList<Thread*>* runQueue;
    tss_t tss __attribute__((aligned(16))); 
};



enum {
	CPUID_ECX_SSE3 = 1 << 0,
	CPUID_ECX_PCLMUL = 1 << 1,
	CPUID_ECX_DTES64 = 1 << 2,
	CPUID_ECX_MONITOR = 1 << 3,
	CPUID_ECX_DS_CPL = 1 << 4,
	CPUID_ECX_VMX = 1 << 5,
	CPUID_ECX_SMX = 1 << 6,
	CPUID_ECX_EST = 1 << 7,
	CPUID_ECX_TM2 = 1 << 8,
	CPUID_ECX_SSSE3 = 1 << 9,
	CPUID_ECX_CID = 1 << 10,
	CPUID_ECX_FMA = 1 << 12,
	CPUID_ECX_CX16 = 1 << 13,
	CPUID_ECX_ETPRD = 1 << 14,
	CPUID_ECX_PDCM = 1 << 15,
	CPUID_ECX_PCIDE = 1 << 17,
	CPUID_ECX_DCA = 1 << 18,
	CPUID_ECX_SSE4_1 = 1 << 19,
	CPUID_ECX_SSE4_2 = 1 << 20,
	CPUID_ECX_x2APIC = 1 << 21,
	CPUID_ECX_MOVBE = 1 << 22,
	CPUID_ECX_POPCNT = 1 << 23,
	CPUID_ECX_AES = 1 << 25,
	CPUID_ECX_XSAVE = 1 << 26,
	CPUID_ECX_OSXSAVE = 1 << 27,
	CPUID_ECX_AVX = 1 << 28,

	CPUID_EDX_FPU = 1 << 0,
	CPUID_EDX_VME = 1 << 1,
	CPUID_EDX_DE = 1 << 2,
	CPUID_EDX_PSE = 1 << 3,
	CPUID_EDX_TSC = 1 << 4,
	CPUID_EDX_MSR = 1 << 5,
	CPUID_EDX_PAE = 1 << 6,
	CPUID_EDX_MCE = 1 << 7,
	CPUID_EDX_CX8 = 1 << 8,
	CPUID_EDX_APIC = 1 << 9,
	CPUID_EDX_SEP = 1 << 11,
	CPUID_EDX_MTRR = 1 << 12,
	CPUID_EDX_PGE = 1 << 13,
	CPUID_EDX_MCA = 1 << 14,
	CPUID_EDX_CMOV = 1 << 15,
	CPUID_EDX_PAT = 1 << 16,
	CPUID_EDX_PSE36 = 1 << 17,
	CPUID_EDX_PSN = 1 << 18,
	CPUID_EDX_CLF = 1 << 19,
	CPUID_EDX_DTES = 1 << 21,
	CPUID_EDX_ACPI = 1 << 22,
	CPUID_EDX_MMX = 1 << 23,
	CPUID_EDX_FXSR = 1 << 24,
	CPUID_EDX_SSE = 1 << 25,
	CPUID_EDX_SSE2 = 1 << 26,
	CPUID_EDX_SS = 1 << 27,
	CPUID_EDX_HTT = 1 << 28,
	CPUID_EDX_TM1 = 1 << 29,
	CPUID_EDX_IA64 = 1 << 30,
	CPUID_EDX_PBE = 1 << 31
};


typedef struct {
	char vendorString[12]; // CPU vendor string
	char nullTerminator; // Acts as a terminator for the vendor string

	uint32_t features_ecx; // CPU features (ecx)
	uint32_t features_edx; // CPU features (edx)
} __attribute__((packed)) cpuid_info_t;

cpuid_info_t CPUID();

static inline void SetCPULocal(CPU* val){
	val->self = val;
	asm volatile("wrmsr" :: "a"((uintptr_t)val & 0xFFFFFFFF) /*Value low*/, "d"(((uintptr_t)val >> 32) & 0xFFFFFFFF) /*Value high*/, "c"(0xC0000102) /*Set Kernel GS Base*/);
	asm volatile("wrmsr" :: "a"((uintptr_t)val & 0xFFFFFFFF) /*Value low*/, "d"(((uintptr_t)val >> 32) & 0xFFFFFFFF) /*Value high*/, "c"(0xC0000101) /*Set Kernel GS Base*/);
}




__attribute__((always_inline)) static inline CPU* GetCPULocal(){
	CPU* ret;
	int intEnable = CheckInterrupts();
	asm("cli");
    asm volatile("swapgs; movq %%gs:0, %0; swapgs;" : "=r"(ret)); // CPU info is 16-byte aligned as per liballoc alignment
	if(intEnable)
		asm("sti");
	return ret;
}

enum {
	BIOSInformation,
	SystemInformation,
	MainboardInformation,
	Enclosure_ChasisInformation,
	ProcessorInformation,
	CacheInformation = 7,
	SystemSlotsInformation = 9,
	PhysicalMemoryArray = 16,
	MemoryDeviceInformation = 17,
	MemoryArrayMappedAddress = 19,
	MemoryDeviceMappedAddress = 20,// (optional as of SMBIOS 2.5)
	SystemBootInformation = 32
};
struct SMBIOSHeader {
 	uint8_t Type;
 	uint8_t Length;
 	uint16_t Handle;
	
};

struct SMBIOSEntryPoint {
 	char EntryPointString[4];    //This is _SM_
 	uchar Checksum;              //This value summed with all the values of the table, should be 0 (overflow)
 	uchar Length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
 	uchar MajorVersion;          //Major Version of SMBIOS
 	uchar MinorVersion;          //Minor Version of SMBIOS
 	ushort MaxStructureSize;     //Maximum size of a SMBIOS Structure (we will se later)
 	uchar EntryPointRevision;    //...
 	char FormattedArea[5];       //...
 	char EntryPointString2[5];   //This is _DMI_
 	uchar Checksum2;             //Checksum for values from EntryPointString2 to the end of table
 	ushort TableLength;          //Length of the Table containing all the structures
 	uint32_t TableAddress;	     //Address of the Table
 	ushort NumberOfStructures;   //Number of structures in the table
 	uchar BCDRevision;           //Unused
};

void GetCpuSpeed();
#endif