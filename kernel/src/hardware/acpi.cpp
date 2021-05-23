#include <acpi.h>

//#include <list.h>
#include <string.h>
#include <kernel.h>
#include <memory.h>
#include <mmu_heap.h>
#include <apic.h>
#include <debug.h>
#include <error.h>
#include <pci.h>
#include <string.h>
#include <madt.h>
#include <ns.h>
#include <hpet.h>
using namespace myos;


acpi_rsdp_t *rsdp = NULL;
acpi_rsdt_t  *rsdt = NULL;
acpi_xsdt_t *xsdt = NULL;
acpi_fadt_t* fadt = NULL;
pci_mcfg_table_t* mcfg = NULL;


static int use_xsdt = 0;
namespace ACPI{
	uint8_t processors[MAX_PROCESSORS];
	int processorCount = 1; // total processors number

	const char* signature = "RSD PTR ";

	List<apic_iso_t*>* isos;

	acpi_xsdp_t* desc;
	acpi_rsdt_t* rsdtHeader;
	acpi_xsdt_t* xsdtHeader;
	acpi_fadt_t* fadt;
	pci_mcfg_table_t* mcfg = nullptr;

	char oem[7];

	void* FindSDT(const char* signature, int index)
	{
		int entries = 0;
		
		if(desc->revision == 2){
			entries = (rsdtHeader->header.length - sizeof(acpi_header_t)) / sizeof(uint64_t); // ACPI 2.0
		} else {
			entries = (rsdtHeader->header.length - sizeof(acpi_header_t)) / sizeof(uint32_t); // ACPI 1.0
		}

		auto getEntry = [](unsigned index) -> uintptr_t { // This will handle differences in ACPI revisions
			if(desc->revision == 2){
				return xsdtHeader->tables[index];
			} else {
				return rsdtHeader->tables[index];
			}
		};

		int _index = 0;

		if(memcmp("DSDT", signature, 4) == 0) return (void*)GetIOMapping(fadt->dsdt);
	
		for (int i = 0; i < entries; i++)
		{
			acpi_header_t* h = (acpi_header_t*)GetIOMapping(getEntry(i));
			if (memcmp(h->signature, signature, 4) == 0 && _index++ == index)
				return h;
		}
	
		// No SDT found
		return NULL;
	}

	int ReadMADT(){
		acpi_madt_t* madt = (acpi_madt_t* )FindSDT("APIC", 0);

		if(!madt){
			DEBUG("Could Not Find MADT");
			return 1;
		}

		acpi_madt_t* madtHeader = (acpi_madt_t*)madt;
		uintptr_t madtEnd = reinterpret_cast<uintptr_t>(madt) + madtHeader->header.length;
		uintptr_t madtEntry = reinterpret_cast<uintptr_t>(madt) + sizeof(acpi_madt_t);

		while(madtEntry < madtEnd){
			acpi_madt_entry_t* entry = (acpi_madt_entry_t*)madtEntry;

			madtEntry += entry->length;
		}

		//Log::Info("[ACPI] System Contains %d Processors", processorCount);

		return 0;
	}

	void Init(){
		if(desc){
			goto success; // Already found
		}

		for(int i = 0; i <= 0x7BFF; i += 16){ // Search first KB for RSDP, the RSDP is aligned on a 16 byte boundary
			if(memcmp(&i,signature,8) == 0){
				desc = ((acpi_xsdp_t*)(i));

				goto success;
			}
		}

		for(int i = 0x80000; i <= 0x9FFFF; i += 16){ // Search further for RSDP
			if(memcmp((void*)GetIOMapping(i),signature,8) == 0){
				desc = ((acpi_xsdp_t*)GetIOMapping(i));

				goto success;
			}
		}

		for(int i = 0xE0000; i <= 0xFFFFF; i += 16){ // Search further for RSDP
			if(memcmp((void*)GetIOMapping(i),signature,8) == 0){
				desc = ((acpi_xsdp_t*)GetIOMapping(i));

				goto success;
			}
		}

		{
		DEBUG("Fatal Erroe: System not ACPI Complaiant.\n");

		//return;
		}

		success:

	//	isos = new List<apic_iso_t*>();

		if(desc->revision == 2){
			rsdtHeader = ((acpi_rsdt_t*)GetIOMapping(desc->xsdt));
			xsdtHeader = ((acpi_xsdt_t*)GetIOMapping(desc->xsdt));
		} else{
			rsdtHeader = ((acpi_rsdt_t*)GetIOMapping(desc->rsdt));
		}

		memcpy(oem,rsdtHeader->header.oem,6);
		oem[6] = 0; // Zero OEM String

		if(debugLevelACPI >= DebugLevelNormal){
			DEBUG("[ACPI] Revision: %d", desc->revision);
		}

		fadt = reinterpret_cast<acpi_fadt_t*>(FindSDT("FACP", 0));

		asm("cli");

		//lai_set_acpi_revision(rsdtHeader->header.revision);
		//lai_create_namespace();

		ReadMADT();
		mcfg = reinterpret_cast<pci_mcfg_table_t*>(FindSDT("MCFG", 0)); // Attempt to find MCFG table for PCI
		if((FindSDT("HPET", 0)))
			DEBUG("hpet exist\n");
		else DEBUG("hept not exist\n");
		asm("sti");
	}

	void SetRSDP(acpi_xsdp_t* p){
		desc = reinterpret_cast<acpi_xsdp_t*>(p);
	}

/*	uint8_t RoutePCIPin(uint8_t bus, uint8_t slot, uint8_t func, uint8_t pin){
		acpi_resource_t res;
		lai_api_error_t e = lai_pci_route_pin(&res, 0, bus, slot, func, pin);
		if(e){
			return 0xFF;
		} else {
			return res.base;
		}
	}
*/
	void Reset(){
		//lai_acpi_reset();
	}
}

extern "C"{
	void *laihost_scan(const char* signature, size_t index){
		return ACPI::FindSDT(signature, index);
	}

	void laihost_log(int level, const char *msg){
		switch(level){
			case LAI_WARN_LOG:
				DEBUG("%s",msg);
				break;
			default:
				if(debugLevelACPI >= DebugLevelNormal){
					DEBUG("%s", msg);
				}
				break;
		}
	}

	/* Reports a fatal error, and halts. */
	void laihost_panic(const char *msg){
		const char* panicReasons[]{"ACPI Error:", msg};
	//	KernelPanic(panicReasons,2);

		for(;;);
	}
	
	void *laihost_malloc(size_t sz){
		return malloc(sz);
	}

	void *laihost_realloc(void* addr, size_t sz, size_t oldsz){
	//	return krealloc(addr, sz);
	}
	void laihost_free(void * addr, size_t sz){
		free(addr);
	}

	void *laihost_map(size_t address, size_t count){
		void* virt = malloc(count / PAGE_SIZE_4K + 1);
		
		//malloc(address, (uintptr_t)virt, count / PAGE_SIZE_4K + 1);

		return virt;
	}

	void laihost_unmap(void* ptr, size_t count){
		// stub
	}

	void laihost_outb(uint16_t port, uint8_t val){
		outb(port, val);
	}

	void laihost_outw(uint16_t port, uint16_t val){
		outw(port, val);
	}

	void laihost_outd(uint16_t port, uint32_t val){
		outl(port, val);
	}

	uint8_t laihost_inb(uint16_t port){
		return inb(port);
	}

	uint16_t laihost_inw(uint16_t port){
		return inw(port);
	}

	uint32_t laihost_ind(uint16_t port){
		return inl(port);
	}
		
	void laihost_sleep(uint64_t ms){
	/*	uint64_t freq = Timer::GetFrequency();
		uint64_t delayInTicks = (freq/1000)*ms;
		uint64_t seconds = Timer::GetSystemUptime();
		uint64_t ticks = Timer::GetTicks();
		uint64_t totalTicks = seconds*freq + ticks;

		for(;;){
			uint64_t totalTicksNew = Timer::GetSystemUptime()*freq + Timer::GetTicks();
			if(totalTicksNew - totalTicks == delayInTicks){
				break;
			}
		}*/
	}

	void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val){
		PCI::PeripheralComponentInterconnectController::ConfigWriteWord(bus, slot, fun, offset, val);
	}
}

#define MEM_PHYS_OFFSET 0xffff800000000000
void init_acpi(void) {
  DEBUG("acpi: Initialising...");

    /* look for the "RSD PTR " signature from 0x80000 to 0xa0000 and from
       0xf0000 to 0x100000 */
    for (size_t i = 0x80000 + MEM_PHYS_OFFSET; i < 0x100000 + MEM_PHYS_OFFSET; i += 16) {
        if (i == 0xa0000 + MEM_PHYS_OFFSET) {
            /* skip video mem and mapped hardware */
            i = 0xe0000 - 16 + MEM_PHYS_OFFSET;
            continue;
        }
        if (!strncmp((char *)i, "RSD PTR ", 8)) {
           DEBUG("acpi: Found RSDP at %X", i);
            rsdp = (struct acpi_rsdp_t *)i;
            goto rsdp_found;
        }
    }

    DEBUG("Non-ACPI compliant system");
    return;

rsdp_found:
    DEBUG ("acpi: ACPI available");

    DEBUG("acpi: Revision: %u", (uint32_t)rsdp->revision);

    if (rsdp->revision >= 2 && rsdp->xsdt_addr) {
        use_xsdt = 1;
       DEBUG("acpi: Found XSDT at %X", ((size_t)rsdp->xsdt_addr + MEM_PHYS_OFFSET));
        xsdt = (struct acpi_xsdt_t *)((size_t)rsdp->xsdt_addr + MEM_PHYS_OFFSET);
    } else {
       DEBUG("acpi: Found RSDT at %X", ((size_t)rsdp->rsdt_addr + MEM_PHYS_OFFSET));
        rsdt = (struct acpi_rsdt_t *)((size_t)rsdp->rsdt_addr + MEM_PHYS_OFFSET);
		
    }


	fadt = reinterpret_cast<acpi_fadt_t*>(acpi_find_sdt("FACP", 0));
	
	if(fadt)
	{
		DEBUG("\n aaa: %x, %x, %x, %x \n",inl(fadt->smi_command_port), fadt->acpi_enable, fadt->acpi_disable,fadt->pm1a_control_block);
		
		void *dsdt_table = acpi_find_sdt("DSDT", 0);
		if (!dsdt_table)
        	DEBUG("unable to find ACPI DSDT.");
    
		// void *dsdt_amls = lai_load_table(dsdt_table, 0);
		// lai_init_state(&state);
		// lai_populate(root_node, dsdt_amls, &state);
		// lai_finalize_state(&state);

		//fadt->smi_command_port |= fadt->acpi_enable;
		//while ((fadt->pm1a_control_block) & 1 == 0);
		//outb(fadt->smi_command_port, fadt->acpi_enable);
		//while (inw(fadt->pm1a_control_block) & 1 == 0);
		//DEBUG("smi_command %x", fadt->smi_command_port);
		//lai_set_acpi_revision(rsdtHeader->header.revision);
		//lai_create_namespace();
	}
	//asm("cli");

	/* Call table inits */
    init_madt();
	mcfg = reinterpret_cast<pci_mcfg_table_t*>(acpi_find_sdt("MCFG", 0)); // Attempt to find MCFG table for PCI				

	hpet = reinterpret_cast<HPET*>(acpi_find_sdt("HPET", 0));
	
	if(hpet)
	{
		hpet_init();
	}
	//asm("sti");
	acpi_find_all_sdt_index();
	DEBUG("init_acpi over \n");
}


/* Find SDT by signature */
void *acpi_find_sdt(const char *signature, int index) {
    struct acpi_header_t *ptr;
    int cnt = 0;

    if (use_xsdt) {
        for (size_t i = 0; i < (xsdt->header.length - sizeof(struct acpi_header_t)) / 8; i++) {
            ptr = (struct acpi_header_t*)((size_t)xsdt->tables[i] + MEM_PHYS_OFFSET);
            if (!strncmp(ptr->signature, signature, 4)) {
                if (cnt++ == index) {
                   DEBUG("acpi: Found \"%s\" at %X", signature, (size_t)ptr);
                    return (void *)ptr;
                }
            }
        }
    } else {
        for (size_t i = 0; i < (rsdt->header.length - sizeof(struct acpi_header_t)) / 4; i++) {
            ptr = (struct acpi_header_t *)((size_t)rsdt->tables[i] + MEM_PHYS_OFFSET);
            if (!strncmp(ptr->signature, signature, 4)) {
                if (cnt++ == index) {
                    DEBUG("acpi: Found \"%s\" at %X", signature, (size_t)ptr);
                    return (void *)ptr;
                }
            }
        }
    }

    DEBUG("acpi: \"%s\" not found\n", signature);
    return NULL;
}


void acpi_find_all_sdt_index() {
    struct acpi_header_t *ptr;
   
	DEBUG("acpi_find_all_sdt_in \t");
    if (use_xsdt) {
        for (size_t i = 0; i < (xsdt->header.length - sizeof(struct acpi_header_t)) / 8; i++) {
            ptr = (struct acpi_header_t*)((size_t)xsdt->tables[i] + MEM_PHYS_OFFSET);
            if (ptr->signature) {               
                DEBUG("acpi: Found \"%s\" at %X \t", ptr->signature, (size_t)ptr);              
            }
        }
    } else{
        for (size_t i = 0; i < (rsdt->header.length - sizeof(struct acpi_header_t)) / 4; i++) {
            ptr = (struct acpi_header_t *)((size_t)rsdt->tables[i] + MEM_PHYS_OFFSET);
            if (ptr->signature) 
			{             
                DEBUG("acpi: Found \"%s\" at %X\t", ptr->signature, (size_t)ptr);              
            }
        }
    }
    DEBUG("\n");
}
