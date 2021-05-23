#include <ns.h>

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

/*

struct lai_instance *lai_current_instance() {
    static struct lai_instance global_instance;
    return &global_instance;
}
void lai_create_namespace(void) {
  
    struct lai_instance *instance = lai_current_instance();

    // we need the FADT
  //  fadt = laihost_scan("FACP", 0);


    lai_nsnode_t *root_node = lai_create_root();

    // Create the namespace with all the objects.
    lai_state_t state;

    // Load the DSDT.
    void *dsdt_table = laihost_scan("DSDT", 0);
    if (!dsdt_table)
        DEBUG("unable to find ACPI DSDT.");
    
    void *dsdt_amls = lai_load_table(dsdt_table, 0);
    lai_init_state(&state);
    lai_populate(root_node, dsdt_amls, &state);
    lai_finalize_state(&state);

    // Load all SSDTs.
    size_t index = 0;
    acpi_aml_t *ssdt_table;
    while ((ssdt_table = laihost_scan("SSDT", index))) {
        void *ssdt_amls = lai_load_table(ssdt_table, index);
        lai_init_state(&state);
        lai_populate(root_node, ssdt_amls, &state);
        lai_finalize_state(&state);
        index++;
    }

    // The PSDT is treated the same way as the SSDT.
    // Scan for PSDTs too for compatibility with some ACPI 1.0 PCs.
    index = 0;
    acpi_aml_t *psdt_table;
    while ((psdt_table = laihost_scan("PSDT", index))) {
        void *psdt_amls = lai_load_table(psdt_table, index);
        lai_init_state(&state);
        lai_populate(root_node, psdt_amls, &state);
        lai_finalize_state(&state);
        index++;
    }

    DEBUG("ACPI namespace created, total of %d predefined objects.", instance->ns_size);
}
*/