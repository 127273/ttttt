#include <madt.h>

#include <mmu_heap.h>
#include <kernel.h>
#include <debug.h>

#include <apic.h>
#include <acpi.h>
#include <kernel.h>

/* search for MADT table */
struct madt_t *madt  = NULL;
void init_madt(void) {
  
    madt= (struct madt_t*)acpi_find_sdt("APIC", 0);
    if(!madt) {
        DEBUG("Could Not Find MADT");
        return;
    }

   /*  madt_local_apics = (struct madt_local_apic_t **)malloc(ACPI_TABLES_MAX * 20 );
  *madt_io_apics =(struct madt_io_apic_t *) malloc(ACPI_TABLES_MAX);
   * madt_isos = (struct madt_iso_t*)malloc(ACPI_TABLES_MAX);
    *madt_nmis = (struct madt_nmi_t *)malloc(ACPI_TABLES_MAX);
 */

	DEBUG("Addr : %x\n", madt->local_controller_addr);
  		acpi_madt_t* madtHeader = (acpi_madt_t*)madt;
		uintptr_t madtEnd = reinterpret_cast<uintptr_t>(madt) + madtHeader->header.length;
		uintptr_t madtEntry = reinterpret_cast<uintptr_t>(madt) + sizeof(acpi_madt_t);
        	while(madtEntry < madtEnd)
			{
			acpi_madt_entry_t* entry = (acpi_madt_entry_t*)madtEntry; 
            switch(entry->type){
			case MADT_LAPIC:
				{
					apic_local_t* localAPIC = (apic_local_t*)entry;

					if(((apic_local_t*)entry)->flags & 0x3) {
						if(localAPIC->apicID == 0) break; // Found the BSP
						
						if(ACPI::processorCount + 1 < MAX_PROCESSORS)
							ACPI::processors[ACPI::processorCount++] = localAPIC->apicID;

					}
				}
				break;
			case MADT_IOAPIC:
				{
					apic_io_t* ioAPIC = (apic_io_t*)entry;			
					if(!ioAPIC->gSIB)
						DEBUG("IO Base:%x", ioAPIC->address);
                        APIC::IO::SetBase(ioAPIC->address);
				}
				break;
			case MADT_ISO :
				{
					apic_iso_t* interruptSourceOverride = (apic_iso_t*)entry;
                    DEBUG("irq :%x, Gsi:%x\t", interruptSourceOverride->irqSource, interruptSourceOverride->gSI);
				
					//APIC::IO::Redirect(interruptSourceOverride->gSI, interruptSourceOverride->irqSource + 0x20, 1 << 8);
					//ACPI::isos->add_back(interruptSourceOverride);
				}
				break;

			case MADT_NMI:
				{
					apic_nmi_t* nonMaskableInterrupt = (apic_nmi_t*)entry;
				    DEBUG("[ACPI] Found NMI, LINT #%d, processor ID: %x \n", \
					nonMaskableInterrupt->lINT, nonMaskableInterrupt->processorID);
				}
				break;

			case MADT_LAPIC_OVERRIDE:
				//apic_local_address_override_t* addressOverride = (apic_local_address_override_t*)entry;
				break;
			default:
				DEBUG("Invalid MADT Entry, Type: %d", entry->type);
				break;
			}

            madtEntry += entry->length;
		}

    /* parse the MADT entries */
    
}

