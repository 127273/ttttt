#pragma once

#include <stdint.h>
#include <types.h>
#include <list.h>

#define ACPI_TABLES_MAX 256
// Device _STA object
#define ACPI_STA_PRESENT        0x01
#define ACPI_STA_ENABLED        0x02
#define ACPI_STA_VISIBLE        0x04
#define ACPI_STA_FUNCTION        0x08
#define ACPI_STA_BATTERY        0x10

// Parsing Resource Templates
#define ACPI_RESOURCE_MEMORY        1
#define ACPI_RESOURCE_IO        2
#define ACPI_RESOURCE_IRQ        3

// IRQ Flags
#define ACPI_SMALL_IRQ_EDGE_TRIGGERED    0x01
#define ACPI_SMALL_IRQ_ACTIVE_LOW        0x08
#define ACPI_SMALL_IRQ_SHARED            0x10
#define ACPI_SMALL_IRQ_WAKE              0x20
#define ACPI_EXTENDED_IRQ_EDGE_TRIGGERED 0x02
#define ACPI_EXTENDED_IRQ_ACTIVE_LOW     0x04
#define ACPI_EXTENDED_IRQ_SHARED         0x08
#define ACPI_EXTENDED_IRQ_WAKE           0x10

typedef struct acpi_resource_t
{
    uint8_t type;

    uint64_t base;            // valid for everything

    uint64_t length;        // valid for I/O and MMIO

    uint8_t address_space;        // these are valid --
    uint8_t bit_width;        // -- only for --
    uint8_t bit_offset;        // -- generic registers

    uint8_t irq_flags;        // valid for IRQs
} acpi_resource_t;

typedef struct acpi_small_irq_t
{
    uint8_t id;
    uint16_t irq_mask;
    uint8_t config;
}__attribute__((packed)) acpi_small_irq_t;

typedef struct acpi_large_irq_t
{
    uint8_t id;
    uint16_t size;
    uint8_t config;
    uint8_t length;
    uint32_t irq;
}__attribute__((packed)) acpi_large_irq_t;


#define LAI_DEBUG_LOG 1
#define LAI_WARN_LOG 2


//FADT Event/Status Fields
#define ACPI_TIMER            0x0001
#define ACPI_BUSMASTER            0x0010
#define ACPI_GLOBAL            0x0020
#define ACPI_POWER_BUTTON        0x0100
#define ACPI_SLEEP_BUTTON        0x0200
#define ACPI_RTC_ALARM            0x0400
#define ACPI_PCIE_WAKE            0x4000
#define ACPI_WAKE            0x8000

// FADT Control Block
#define ACPI_ENABLED            0x0001
#define ACPI_SLEEP            0x2000

#define ACPI_GAS_MMIO            0
#define ACPI_GAS_IO            1
#define ACPI_GAS_PCI            2

// OpRegion Address Spaces
#define ACPI_OPREGION_MEMORY			0x00
#define ACPI_OPREGION_IO			0x01
#define ACPI_OPREGION_PCI			0x02
#define ACPI_OPREGION_EC			0x03
#define ACPI_OPREGION_SMBUS			0x04
#define ACPI_OPREGION_CMOS			0x05
#define ACPI_OPREGION_OEM			0x80


#define  MAX_PROCESSORS 0xff

typedef struct acpi_uuid_t
{

}__attribute__((packed)) acpi_uuid_t;

typedef struct acpi_rsdp_t
{
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t revision;
    uint32_t rsdt_addr;
    /* ver 2.0 only */
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t reserved[3];
}__attribute__((packed)) acpi_rsdp_t;

typedef struct acpi_header_t
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem[6];
    char oem_table[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
}__attribute__((packed)) acpi_header_t;

typedef struct acpi_rsdt_t
{
    acpi_header_t header;
    uint32_t tables[];
}__attribute__((packed)) acpi_rsdt_t;

typedef struct acpi_xsdt_t
{
    acpi_header_t header;
    uint64_t tables[];
}__attribute__((packed)) acpi_xsdt_t;

typedef struct acpi_xsdp_t
{
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t revision;
    uint32_t rsdt;
    uint32_t length;
    uint64_t xsdt;
    uint8_t extended_checksum;
}__attribute__((packed)) acpi_xsdp_t;

typedef struct acpi_gas_t 
{
}__attribute__((packed)) acpi_gas_t ;

typedef struct acpi_fadt_t
{
    acpi_header_t header;
    uint32_t firmware_control;
    uint32_t dsdt;        // pointer to dsdt

    uint8_t reserved;

    uint8_t profile;
    uint16_t sci_irq;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_control;
    uint32_t pm1a_event_block;
    uint32_t pm1b_event_block;
    uint32_t pm1a_control_block;
    uint32_t pm1b_control_block;
    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;
    uint8_t pm_timer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t cstate_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;

    // cmos registers
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    // ACPI 2.0 fields
    uint16_t iapc_boot_flags;
    uint8_t reserved2;
    uint32_t flags;

    acpi_gas_t reset_register;
    uint8_t reset_command;
    uint16_t arm_boot_flags;
    uint8_t minor_version;

    uint64_t x_firmware_control;
    uint64_t x_dsdt;

    acpi_gas_t x_pm1a_event_block;
    acpi_gas_t x_pm1b_event_block;
    acpi_gas_t x_pm1a_control_block;
    acpi_gas_t x_pm1b_control_block;
    acpi_gas_t x_pm2_control_block;
    acpi_gas_t x_pm_timer_block;
    acpi_gas_t x_gpe0_block;
    acpi_gas_t x_gpe1_block;
}__attribute__((packed)) acpi_fadt_t;


typedef struct acpi_ecdt_t {
    acpi_header_t header;
    struct acpi_gas_t ec_control;
    struct acpi_gas_t ec_data;
    uint32_t uid;
    uint8_t gpe_bit;
    uint8_t ec_id[];
}__attribute__((packed)) acpi_ecdt_t;

typedef struct acpi_aml_t        // AML tables, DSDT and SSDT
{
    acpi_header_t header;
    uint8_t data[];
}__attribute__((packed)) acpi_aml_t;

typedef struct acpi_srat_t 
{
    
}__attribute__((packed));

typedef struct acpi_rasf_t 
{
    
}__attribute__((packed));

typedef struct MADT{
  acpi_header_t header;
  uint32_t localAPICAddress;
  uint32_t flags;
} __attribute__ ((packed)) acpi_madt_t;

typedef struct MADTEntry{
  uint8_t type;
  uint8_t length;
} __attribute__ ((packed)) acpi_madt_entry_t;

typedef struct LocalAPIC{ // Local APIC - Type 0
  acpi_madt_entry_t entry; // MADT Entry Structure
  uint8_t processorID; // ACPI Processor ID
  uint8_t apicID; // APIC ID
  uint32_t flags; // Flags - (bit 0 = enabled, bit 1 = online capable)
} __attribute__ ((packed)) apic_local_t;

typedef struct IOAPIC{ // I/O APIC - Type 1
  acpi_madt_entry_t entry; // MADT Entry Structure
  uint8_t apicID; // APIC ID
  uint8_t reserved; // Reserved
  uint32_t address; // Address of I/O APIC
  uint32_t gSIB; // Global System Interrupt Base
} __attribute__ ((packed)) apic_io_t;

typedef struct ISO{ // Interrupt Source Override - Type 2
  acpi_madt_entry_t entry; // MADT Entry Structure
  uint8_t busSource; // Bus Source
  uint8_t irqSource; // IRQ Source
  uint32_t gSI; // Global System Interrupt
  uint16_t flags; // Flags
} __attribute__ ((packed)) apic_iso_t;

typedef struct NMI{ // Non-maskable Interrupt - Type 4
  acpi_madt_entry_t entry; // MADT Entry Structure
  uint8_t processorID; // ACPI Processor ID (255 for all processors)
  uint16_t flags; // Flags
  uint8_t lINT; // LINT Number (0 or 1)
} __attribute__ ((packed)) apic_nmi_t;

typedef struct LocalAPICAddressOverride { // Local APIC Address Override - Type 5
  acpi_madt_entry_t entry; // MADT Entry Structure
  uint16_t reserved; // Reserved
  uint64_t address; // 64-bit Address of Local APIC
} __attribute__ ((packed)) apic_local_address_override_t;

typedef struct PCIMCFGBaseAddress{
  uint64_t baseAddress; // Base address of configuration space
  uint16_t segmentGroupNumber; // PCI Segment group number
  uint8_t startPCIBusNumber; // Start PCI bus number decoded by this host bridge
  uint8_t endPCIBusNumber; // End PCI bus number decoded by this host bridge 
  uint32_t reserved;
} __attribute__ ((packed)) pci_mcfg_base_adress_t;

typedef struct PCIMCFG {
  acpi_header_t header;
  uint64_t reserved;
  PCIMCFGBaseAddress baseAddresses[];
} __attribute__ ((packed)) pci_mcfg_table_t;


namespace ACPI{
  extern uint8_t processors[];
  extern int processorCount;
  
  extern List<apic_iso_t*>* isos;

	extern acpi_xsdp_t* desc;
	extern acpi_rsdt_t* rsdtHeader;
  extern pci_mcfg_table_t* mcfg;

	void Init();
  void SetRSDP(acpi_xsdp_t* p);
  void SetRSDP(acpi_xsdp_t* p);

  uint8_t RoutePCIPin(uint8_t bus, uint8_t slot, uint8_t func, uint8_t pin);

  void Reset();
}

extern int strncmp(const char *s1, const char *s2, size_t n);
void init_acpi(void) ;


extern acpi_rsdp_t *rsdp;
extern acpi_rsdt_t  *rsdt ;
extern acpi_xsdt_t *xsdt ;

void init_acpi(void);
void *acpi_find_sdt(const char *, int);
void acpi_find_all_sdt_index();
void* FindSDT(const char* signature, int index);