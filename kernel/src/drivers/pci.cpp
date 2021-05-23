#include <pci.h>

#include <string.h>
#include <kernel.h>
#include <x86.h>
#include <sound/intelHD.h>
#include <mmu_heap.h>
#include <types.h>
#include <drivers/usb/xchi.h>
#include <drivers/net/i8254/i8254.h>
uint32_t pci_size_map[100];
pci_dev_t dev_zero = {0};
/*
 * Given a pci device(32-bit vars containing info about bus, device number, and function number), a field(what u want to read from the config space)
 * Read it for me !
 * */
uint32_t pci_read(pci_dev_t dev, uint32_t field)
{
	// Only most significant 6 bits of the field
	dev.field_num = (field & 0xFC) >> 2;
	dev.enable = 1;
	outpd(PCI_CONFIG_ADDRESS, dev.bits);

	// What size is this field supposed to be ?
	uint32_t size = pci_size_map[field];
	if (size == 1)
	{
		// Get the first byte only, since it's in little endian, it's actually the 3rd byte
		uint8_t t = inp(PCI_CONFIG_DATA + (field & 3));
		return t;
	}
	else if (size == 2)
	{
		uint16_t t = inpw(PCI_CONFIG_DATA + (field & 2));
		return t;
	}
	else if (size == 4)
	{
		// Read entire 4 bytes
		uint32_t t = inpd(PCI_CONFIG_DATA);
		return t;
	}
	return 0xffff;
}

/*
 * Write pci field
 * */
void pci_write(pci_dev_t dev, uint32_t field, uint32_t value)
{
	dev.field_num = (field & 0xFC) >> 2;
	dev.enable = 1;
	// Tell where we want to write
	outpd(PCI_CONFIG_ADDRESS, dev.bits);
	// Value to write
	outpd(PCI_CONFIG_DATA, value);
}

/*
 * Get device type (i.e, is it a bridge, ide controller ? mouse controller? etc)
 * */
uint32_t get_device_type(pci_dev_t dev)
{
	uint32_t t = pci_read(dev, PCI_CLASS) << 8;
	return t | pci_read(dev, PCI_SUBCLASS);
}

/*
 * Get secondary bus from a PCI bridge device
 * */
uint32_t get_secondary_bus(pci_dev_t dev)
{
	return pci_read(dev, PCI_SECONDARY_BUS);
}

/*
 * Is current device an end point ? PCI_HEADER_TYPE 0 is end point
 * */
uint32_t pci_reach_end(pci_dev_t dev)
{
	uint32_t t = pci_read(dev, PCI_HEADER_TYPE);
	return !t;
}

/*
 * The following three functions are basically doing recursion, enumerating each and every device connected to pci
 * We start with the primary bus 0, which has 8 function, each of the function is actually a bus
 * Then, each bus can have 8 devices connected to it, each device can have 8 functions
 * When we gets to enumerate the function, check if the vendor id and device id match, if it does, we've found our device !
 **/

/*
 * Scan function
 * */
pci_dev_t pci_scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, uint32_t device_type)
{
	pci_dev_t dev = {0};
	dev.bus_num = bus;
	dev.device_num = device;
	dev.function_num = function;
	// If it's a PCI Bridge device, get the bus it's connected to and keep searching
	if (get_device_type(dev) == PCI_TYPE_BRIDGE)
	{
		pci_scan_bus(vendor_id, device_id, get_secondary_bus(dev), device_type);
	}
	// If type matches, we've found the device, just return it
	// if(device_type == -1 || device_type == get_device_type(dev)) {
	if (device_type == 0xFFFFFFFF || device_type == get_device_type(dev))
	{
		uint32_t devid = pci_read(dev, PCI_DEVICE_ID);
		uint32_t vendid = pci_read(dev, PCI_VENDOR_ID);
		if (devid == device_id && vendor_id == vendid)
			return dev;
	}
	return dev_zero;
}

/*
 * Scan device
 * */
pci_dev_t pci_scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t device_type)
{
	pci_dev_t dev = {0};
	dev.bus_num = bus;
	dev.device_num = device;

	if (pci_read(dev, PCI_VENDOR_ID) == PCI_NONE)
		return dev_zero;

	pci_dev_t t = pci_scan_function(vendor_id, device_id, bus, device, 0, device_type);
	if (t.bits)
		return t;

	if (pci_reach_end(dev))
		return dev_zero;

	for (int function = 1; function < FUNCTION_PER_DEVICE; function++)
	{
		if (pci_read(dev, PCI_VENDOR_ID) != PCI_NONE)
		{
			t = pci_scan_function(vendor_id, device_id, bus, device, function, device_type);
			if (t.bits)
				return t;
		}
	}
	return dev_zero;
}
/*
 * Scan bus
 * */
pci_dev_t pci_scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device_type)
{
	for (int device = 0; device < DEVICE_PER_BUS; device++)
	{
		pci_dev_t t = pci_scan_device(vendor_id, device_id, bus, device, device_type);
		if (t.bits)
			return t;
	}
	return dev_zero;
}

/*
 * Device driver use this function to get its device object(given unique vendor id and device id)
 * */
pci_dev_t pci_get_device(uint16_t vendor_id, uint16_t device_id, uint32_t device_type)
{

	pci_dev_t t = pci_scan_bus(vendor_id, device_id, 0, device_type);
	if (t.bits)
		return t;

	// Handle multiple pci host controllers

	if (pci_reach_end(dev_zero))
	{
		DEBUG("[PCI]: PCI Get device failed...\n");
	}
	for (int function = 1; function < FUNCTION_PER_DEVICE; function++)
	{
		pci_dev_t dev = {0};
		dev.function_num = function;

		if (pci_read(dev, PCI_VENDOR_ID) == PCI_NONE)
			break;
		t = pci_scan_bus(vendor_id, device_id, function, device_type);
		if (t.bits)
			return t;
	}
	return dev_zero;
}

/*
 * PCI Init, filling size for each field in config space
 * */
void init_kernel_pci()
{
	// Init size map
	pci_size_map[PCI_VENDOR_ID] = 2;
	pci_size_map[PCI_DEVICE_ID] = 2;
	pci_size_map[PCI_COMMAND] = 2;
	pci_size_map[PCI_STATUS] = 2;
	pci_size_map[PCI_SUBCLASS] = 1;
	pci_size_map[PCI_CLASS] = 1;
	pci_size_map[PCI_CACHE_LINE_SIZE] = 1;
	pci_size_map[PCI_LATENCY_TIMER] = 1;
	pci_size_map[PCI_HEADER_TYPE] = 1;
	pci_size_map[PCI_BIST] = 1;
	pci_size_map[PCI_BAR0] = 4;
	pci_size_map[PCI_BAR1] = 4;
	pci_size_map[PCI_BAR2] = 4;
	pci_size_map[PCI_BAR3] = 4;
	pci_size_map[PCI_BAR4] = 4;
	pci_size_map[PCI_BAR5] = 4;
	pci_size_map[PCI_INTERRUPT_LINE] = 1;
	pci_size_map[PCI_SECONDARY_BUS] = 1;
}

#include <pci.h>

using namespace myos;
using namespace myos::drivers;
using namespace myos::PCI;

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController(drivers::UsefulDevices *ufdrv, myos::hardware::InterruptManager* interrupts)

{


	for (int bus = 0; bus < 8; bus++)
	{
		for (int device = 0; device < 32; device++)
		{
			int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
			for (int function = 0; function < numFunctions; function++)
			{
				PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);

				if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
					continue;

				const char *str1 = pci_strvendor(dev.vendor_id);
				DEBUG("vendor_id: %x, %s  ", dev.vendor_id, str1);

				const char *str2 = pci_strclass(dev.class_id << 8 + dev.subclass_id);
				DEBUG("classid: %s ", str2);

				DEBUG("device_id 0x%x ", dev.device_id);

				PCIDevice pci;
				pci.bus = dev.bus;
				pci.func = dev.function;
				pci.slot = dev.device;
				dev.BaseAddress = pci.GetBaseAddressRegister(0);

				dev.BaseAddress = GetBaseAddressRegister(dev.bus, dev.device, dev.function, 0);
				DEBUG("address %x  interr: %x\n", dev.BaseAddress, dev.InterruptLine);
				if (dev.BaseAddress != 0)
				{
					dev.BaseAddress = TO_VMA_U64(dev.BaseAddress);
				
					//	Driver *driver = GetDriver(dev, interrupts);
					//	if (driver != NULL)
					//		driverManager->AddDriver(driver, dev);
					//	else
					ufdrv->AddDriver(dev);
					
				}
		
				//
			}
		}
	}
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{
	
}


uintptr_t PeripheralComponentInterconnectController::GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t idx)
{
	uintptr_t bar = PeripheralComponentInterconnectController::ConfigReadDword(bus, device, function, PCIBAR0 + (idx * sizeof(uint32_t)));
	if (!(bar & 0x1) /* Not IO */ && bar & 0x4 /* 64-bit */ && idx < 5)
	{
		DEBUG("64 bit");
		bar |= static_cast<uintptr_t>(PeripheralComponentInterconnectController::ConfigReadDword(bus, device, function, PCIBAR0 + ((bar + 1) * sizeof(uint32_t)))) << 32;
	}


	return (bar & 0x1) ? (bar & 0xFFFFFFFFFFFFFFFC) : (bar & 0xFFFFFFFFFFFFFFF0);
}

uint32_t PeripheralComponentInterconnectController::ConfigReadDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000);

	outl(0xCF8, address);

	uint32_t data;
	data = inl(0xCFC);

	return data;
}

uint16_t PeripheralComponentInterconnectController::ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000);

	outl(0xCF8, address);

	uint16_t data;
	data = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);

	return data;
}

uint8_t PeripheralComponentInterconnectController::ConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000);

	outl(0xCF8, address);

	uint8_t data;
	data = (uint8_t)((inl(0xCFC) >> ((offset & 3) * 8)) & 0xff);

	return data;
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint16_t bus, uint16_t device)
{
	return ConfigReadDword(bus, device, 0, 0x0E) & (1 << 7);
}

void PeripheralComponentInterconnectController::ConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data)
{
	uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000);

	outl(0xCF8, address);
	outl(0xCFC, (inl(0xCFC) & (~(0xFFFF << ((offset & 2) * 8)))) | (static_cast<uint32_t>(data) << ((offset & 2) * 8)));
}

void PeripheralComponentInterconnectController::ConfigWriteByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t data)
{
	uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000);

	outl(0xCF8, address);
	outb(0xCFC, (inl(0xCFC) & (~(0xFF << ((offset & 3) * 8)))) | (static_cast<uint32_t>(data) << ((offset & 3) * 8)));
}

uint16_t GetVendor(uint8_t bus, uint8_t slot, uint8_t func)
{
	uint16_t vendor;

	vendor = PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, 0);
	return vendor;
}

uint16_t GetDeviceID(uint8_t bus, uint8_t slot, uint8_t func)
{
	uint16_t id;

	id = PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, 2);
	return id;
}

bool PeripheralComponentInterconnectController::CheckDevice(uint8_t bus, uint8_t device, uint8_t func)
{
	if (GetVendor(bus, device, func) == 0xFFFF)
	{
		return false;
	}

	return true;
}

bool PeripheralComponentInterconnectController::FindDevice(uint16_t deviceID, uint16_t vendorID)
{


	return false;
}

bool PeripheralComponentInterconnectController::FindGenericDevice(uint16_t classCode, uint16_t subclass)
{
	/*for (unsigned i = 0; i < devices->get_length(); i++)
	{
		if (devices->get_at(i).classCode == classCode && devices->get_at(i).subclass == subclass)
		{
			return true;
		}
	}*/

	return false;
}




int AddDevice(int bus, int slot, int func)
{
	PCIDevice device;

	device.vendorID = GetVendor(bus, slot, func);
	device.deviceID = GetDeviceID(bus, slot, func);

	device.bus = bus;
	device.slot = slot;
	device.func = func;

	device.UpdateClass();

	/*Log::Info("Found Device! Vendor: ");
		Log::Write(device.vendorID);
		Log::Write(", Device: ");
		Log::Write(device.deviceID);
		Log::Write(", Class: ");
		Log::Write(device.classCode);
		Log::Write(", Subclass: ");
		Log::Write(device.subclass);*/

	//device.capabilities = new Vector<uint16_t>();
	if (device.Status() & PCI_STATUS_CAPABILITIES)
	{
		uint8_t ptr = PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, PCICapabilitiesPointer) & 0xFC;
		uint16_t cap = PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, ptr);
		do
		{
			//Log::Info("PCI Capability: %x", cap & 0xFF);

			if ((cap & 0xFF) == PCICapabilityIDs::PCICapMSI)
			{
				device.msiPtr = ptr;
				device.msiCapable = true;
				device.msiCap.register0 = PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, ptr);
				device.msiCap.register1 = PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, ptr + sizeof(uint32_t));
				device.msiCap.register2 = PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, ptr + sizeof(uint32_t) * 2);

				if (device.msiCap.msiControl & PCI_CAP_MSI_CONTROL_64)
				{ // 64-bit capable
					device.msiCap.data64 = PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, ptr + sizeof(uint32_t) * 3);
				}
			}

			ptr = (cap >> 8);
			cap = PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, ptr);

			//device.capabilities->add_back(cap & 0xFF);
		} while ((cap >> 8));
	}

	//int ret = devices->get_length();
	//devices->add_back(device);
	//return ret;
	return 0;
}


PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
	PeripheralComponentInterconnectDeviceDescriptor result;

	result.bus = bus;
	result.device = device;
	result.function = function;

	result.vendor_id = ConfigReadWord(bus, device, function, 0x00);
	result.device_id = ConfigReadWord(bus, device, function, 0x02);

	result.class_id = ConfigReadWord(bus, device, function, 0x0b);
	result.subclass_id = ConfigReadWord(bus, device, function, 0x0a);
	result.ProgIF = ConfigReadWord(bus, device, function, 0x09);

	result.revision_id = ConfigReadWord(bus, device, function, 0x08);
	result.InterruptLine = ConfigReadWord(bus, device, function, 0x3c) ;

	/*  result.MemoryBaseAddress0 	= Read(bus, device, function, 0x1C);
	result.MemoryLimit0 		= Read(bus, device, function, 0x20);
	result.MemoryBaseAddress1 	= Read(bus, device, function, 0x24);
	result.MemoryLimit1 		= Read(bus, device, function, 0x28);
	result.IOBaseAddress0 		= Read(bus, device, function, 0x2c);
	result.IOLimit0				= Read(bus, device, function, 0x30);	
	result.IOBaseAddress1 		= Read(bus, device, function, 0x34);
	result.IOLimit1				= Read(bus, device, function, 0x38);	
*/

	return result;
}

/* linux 源码的 */
#define DEVICE(vid, did, name)                                 \
	{                                                          \
		PCI_VENDOR_ID_##vid, PCI_DEVICE_ID_##did, (name), 0xff \
	}

#define BRIDGE(vid, did, name, bridge)                             \
	{                                                              \
		PCI_VENDOR_ID_##vid, PCI_DEVICE_ID_##did, (name), (bridge) \
	}

struct pci_dev_info dev_info[] = {
	DEVICE(NCR, NCR_53C810, "53c810"),
	DEVICE(NCR, NCR_53C815, "53c815"),
	DEVICE(NCR, NCR_53C820, "53c820"),
	DEVICE(NCR, NCR_53C825, "53c825"),
	DEVICE(ADAPTEC, ADAPTEC_2940, "2940"),
	DEVICE(ADAPTEC, ADAPTEC_294x, "294x"),
	DEVICE(DPT, DPT, "SmartCache/Raid"),
	DEVICE(S3, S3_864_1, "Vision 864-P"),
	DEVICE(S3, S3_864_2, "Vision 864-P"),
	DEVICE(S3, S3_868, "Vision 868"),
	DEVICE(S3, S3_928, "Vision 928-P"),
	DEVICE(S3, S3_964_1, "Vision 964-P"),
	DEVICE(S3, S3_964_2, "Vision 964-P"),
	DEVICE(S3, S3_811, "Trio32/Trio64"),
	DEVICE(S3, S3_968, "Vision 968"),
	DEVICE(OPTI, OPTI_82C822, "82C822"),
	DEVICE(OPTI, OPTI_82C621, "82C621"),
	DEVICE(OPTI, OPTI_82C557, "82C557"),
	DEVICE(OPTI, OPTI_82C558, "82C558"),
	BRIDGE(UMC, UMC_UM8881F, "UM8881F", 0x02),
	BRIDGE(UMC, UMC_UM8891A, "UM8891A", 0x01),
	DEVICE(UMC, UMC_UM8886F, "UM8886F"),
	DEVICE(UMC, UMC_UM8673F, "UM8673F"),
	DEVICE(DEC, DEC_TULIP, "DC21040"),
	DEVICE(DEC, DEC_TULIP_FAST, "DC21040"),
	DEVICE(DEC, DEC_FDDI, "DEFPA"),
	DEVICE(DEC, DEC_BRD, "DC21050"),
	DEVICE(MATROX, MATROX_MGA_2, "Atlas PX2085"),
	DEVICE(MATROX, MATROX_MGA_IMP, "MGA Impression"),
	DEVICE(INTEL, INTEL_82378, "82378IB"),
	BRIDGE(INTEL, INTEL_82424, "82424ZX Saturn", 0x00),
	DEVICE(INTEL, INTEL_82375, "82375EB"),
	BRIDGE(INTEL, INTEL_82434, "82434LX Mercury/Neptune", 0x00),
	DEVICE(INTEL, INTEL_82430, "82430ZX Aries"),
	DEVICE(SMC, SMC_37C665, "FDC 37C665"),
	DEVICE(ATI, ATI_M32, "Mach 32"),
	DEVICE(ATI, ATI_M64, "Mach 64"),
	DEVICE(WEITEK, WEITEK_P9000, "P9000"),
	DEVICE(WEITEK, WEITEK_P9100, "P9100"),
	DEVICE(CIRRUS, CIRRUS_5430, "GD 5430"),
	DEVICE(CIRRUS, CIRRUS_5434_4, "GD 5434"),
	DEVICE(CIRRUS, CIRRUS_5434_8, "GD 5434"),
	DEVICE(CIRRUS, CIRRUS_6729, "CL 6729"),
	DEVICE(BUSLOGIC, BUSLOGIC_946C, "946C"),
	DEVICE(BUSLOGIC, BUSLOGIC_946C_2, "946C"),
	DEVICE(N9, N9_I128, "Imagine 128"),
	DEVICE(AI, AI_M1435, "M1435"),
	DEVICE(AL, AL_M1445, "M1445"),
	DEVICE(AL, AL_M1449, "M1449"),
	DEVICE(AL, AL_M1451, "M1451"),
	DEVICE(TSENG, TSENG_W32P_2, "ET4000W32P"),
	DEVICE(TSENG, TSENG_W32P_b, "ET4000W32P rev B"),
	DEVICE(TSENG, TSENG_W32P_c, "ET4000W32P rev C"),
	DEVICE(TSENG, TSENG_W32P_d, "ET4000W32P rev D"),
	DEVICE(CMD, CMD_640, "640A"),
	DEVICE(VISION, VISION_QD8500, "QD-8500"),
	DEVICE(VISION, VISION_QD8580, "QD-8580"),
	DEVICE(AMD, AMD_LANCE, "79C970"),
	DEVICE(AMD, AMD_SCSI, "53C974"),
	DEVICE(VLSI, VLSI_82C593, "82C593-FC1"),
	DEVICE(VLSI, VLSI_82C592, "82C592-FC1"),
	DEVICE(ADL, ADL_2301, "2301"),
	DEVICE(SYMPHONY, SYMPHONY_101, "82C101"),
	DEVICE(TRIDENT, TRIDENT_9420, "TG 9420"),
	DEVICE(TRIDENT, TRIDENT_9440, "TG 9440"),
	DEVICE(CONTAQ, CONTAQ_82C599, "82C599"),
	DEVICE(NS, NS_87410, "87410"),
	DEVICE(VIA, VIA_82C505, "VT 82C505"),
	DEVICE(VIA, VIA_82C576, "VT 82C576 3V"),
	DEVICE(VIA, VIA_82C561, "VT 82C561"),
	DEVICE(SI, SI_496, "85C496"),
	DEVICE(SI, SI_501, "85C501"),
	DEVICE(SI, SI_503, "85C503"),
	DEVICE(SI, SI_601, "85C601"),
	DEVICE(LEADTEK, LEADTEK_805, "S3 805"),
	DEVICE(IMS, IMS_8849, "8849"),
	DEVICE(ZEINET, ZEINET_1221, "1221"),
	DEVICE(EF, EF_ATM, "155P-MF1"),
	DEVICE(HER, HER_STING, "Stingray"),
	DEVICE(HER, HER_STING, "Stingray"),
	DEVICE(ATRONICS, ATRONICS_2015, "IDE-2015PL"),
	DEVICE(CT, CT_65545, "65545"),
	DEVICE(FD, FD_36C70, "TMC-18C30"),
	DEVICE(WINBOND, WINBOND_83769, "W83769F"),
	DEVICE(3COM, 3COM_3C590, "3C590 10bT"),
	DEVICE(3COM, 3COM_3C595TX, "3C595 100bTX"),
	DEVICE(3COM, 3COM_3C595T4, "3C595 100bT4"),
	DEVICE(3COM, 3COM_3C595MII, "3C595 100b-MII"),
	DEVICE(PROMISE, PROMISE_5300, "DC5030"),
	DEVICE(QLOGIC, QLOGIC_ISP1020, "ISP1020"),
	DEVICE(QLOGIC, QLOGIC_ISP1022, "ISP1022"),
	DEVICE(X, X_AGX016, "ITT AGX016")};

char *pci_strbioserr(int error)
{
	switch (error)
	{
	case PCIBIOS_SUCCESSFUL:
		return "SUCCESSFUL";
	case PCIBIOS_FUNC_NOT_SUPPORTED:
		return "FUNC_NOT_SUPPORTED";
	case PCIBIOS_BAD_VENDOR_ID:
		return "SUCCESSFUL";
	case PCIBIOS_DEVICE_NOT_FOUND:
		return "DEVICE_NOT_FOUND";
	case PCIBIOS_BAD_REGISTER_NUMBER:
		return "BAD_REGISTER_NUMBER";
	case PCIBIOS_SET_FAILED:
		return "SET_FAILED";
	case PCIBIOS_BUFFER_TOO_SMALL:
		return "BUFFER_TOO_SMALL";
	default:
		return "Unknown error status";
	}
}

const char *pci_strclass(unsigned int strclass)
{
	switch (strclass)
	{
	case PCI_CLASS_NOT_DEFINED:
		return "Non-VGA device";
	case PCI_CLASS_NOT_DEFINED_VGA:
		return "VGA compatible device";

	case PCI_CLASS_STORAGE_SCSI:
		return "SCSI storage controller";
	case PCI_CLASS_STORAGE_IDE:
		return "IDE interface";
	case PCI_CLASS_STORAGE_FLOPPY:
		return "Floppy disk controller";
	case PCI_CLASS_STORAGE_IPI:
		return "IPI bus controller";
	case PCI_CLASS_STORAGE_RAID:
		return "RAID bus controller";
	case PCI_CLASS_STORAGE_OTHER:
		return "Unknown mass storage controller";

	case PCI_CLASS_NETWORK_ETHERNET:
		return "Ethernet controller";
	case PCI_CLASS_NETWORK_TOKEN_RING:
		return "Token ring network controller";
	case PCI_CLASS_NETWORK_FDDI:
		return "FDDI network controller";
	case PCI_CLASS_NETWORK_ATM:
		return "ATM network controller";
	case PCI_CLASS_NETWORK_OTHER:
		return "Network controller";

	case PCI_CLASS_DISPLAY_VGA:
		return "VGA compatible controller";
	case PCI_CLASS_DISPLAY_XGA:
		return "XGA compatible controller";
	case PCI_CLASS_DISPLAY_OTHER:
		return "Display controller";

	case PCI_CLASS_MULTIMEDIA_VIDEO:
		return "Multimedia video controller";
	case PCI_CLASS_MULTIMEDIA_AUDIO:
		return "Multimedia audio controller";
	case PCI_CLASS_MULTIMEDIA_OTHER:
		return "Multimedia controller";

	case PCI_CLASS_MEMORY_RAM:
		return "RAM memory";
	case PCI_CLASS_MEMORY_FLASH:
		return "FLASH memory";
	case PCI_CLASS_MEMORY_OTHER:
		return "Memory";

	case PCI_CLASS_BRIDGE_HOST:
		return "Host bridge";
	case PCI_CLASS_BRIDGE_ISA:
		return "ISA bridge";
	case PCI_CLASS_BRIDGE_EISA:
		return "EISA bridge";
	case PCI_CLASS_BRIDGE_MC:
		return "MicroChannel bridge";
	case PCI_CLASS_BRIDGE_PCI:
		return "PCI bridge";
	case PCI_CLASS_BRIDGE_PCMCIA:
		return "PCMCIA bridge";
	case PCI_CLASS_BRIDGE_NUBUS:
		return "NuBus bridge";
	case PCI_CLASS_BRIDGE_CARDBUS:
		return "CardBus bridge";
	case PCI_CLASS_BRIDGE_OTHER:
		return "Bridge";

	case PCI_CLASS_COMMUNICATION_SERIAL:
		return "Serial controller";
	case PCI_CLASS_COMMUNICATION_PARALLEL:
		return "Parallel controller";
	case PCI_CLASS_COMMUNICATION_OTHER:
		return "Communication controller";

	case PCI_CLASS_SYSTEM_PIC:
		return "PIC";
	case PCI_CLASS_SYSTEM_DMA:
		return "DMA controller";
	case PCI_CLASS_SYSTEM_TIMER:
		return "Timer";
	case PCI_CLASS_SYSTEM_RTC:
		return "RTC";
	case PCI_CLASS_SYSTEM_OTHER:
		return "System peripheral";

	case PCI_CLASS_INPUT_KEYBOARD:
		return "Keyboard controller";
	case PCI_CLASS_INPUT_PEN:
		return "Digitizer Pen";
	case PCI_CLASS_INPUT_MOUSE:
		return "Mouse controller";
	case PCI_CLASS_INPUT_OTHER:
		return "Input device controller";

	case PCI_CLASS_DOCKING_GENERIC:
		return "Generic Docking Station";
	case PCI_CLASS_DOCKING_OTHER:
		return "Docking Station";

	case PCI_CLASS_PROCESSOR_386:
		return "386";
	case PCI_CLASS_PROCESSOR_486:
		return "486";
	case PCI_CLASS_PROCESSOR_PENTIUM:
		return "Pentium";
	case PCI_CLASS_PROCESSOR_ALPHA:
		return "Alpha";
	case PCI_CLASS_PROCESSOR_POWERPC:
		return "Power PC";
	case PCI_CLASS_PROCESSOR_CO:
		return "Co-processor";

	case PCI_CLASS_SERIAL_FIREWIRE:
		return "FireWire (IEEE 1394)";
	case PCI_CLASS_SERIAL_ACCESS:
		return "ACCESS Bus";
	case PCI_CLASS_SERIAL_SSA:
		return "SSA";
	case PCI_CLASS_SERIAL_FIBER:
		return "Fiber Channel";
		/* need more information */
	default:
		return "Unknown class";
	}
}

const char *pci_strvendor(unsigned int vendor)
{
	switch (vendor)
	{
	case PCI_VENDOR_ID_NCR:
		return "NCR";
	case PCI_VENDOR_ID_ADAPTEC:
		return "Adaptec";
	case PCI_VENDOR_ID_DPT:
		return "DPT";
	case PCI_VENDOR_ID_S3:
		return "S3 Inc.";
	case PCI_VENDOR_ID_OPTI:
		return "OPTI";
	case PCI_VENDOR_ID_UMC:
		return "UMC";
	case PCI_VENDOR_ID_DEC:
		return "DEC";
	case PCI_VENDOR_ID_MATROX:
		return "Matrox";
	case PCI_VENDOR_ID_INTEL:
		return "Intel";
	case PCI_VENDOR_ID_SMC:
		return "SMC";
	case PCI_VENDOR_ID_ATI:
		return "ATI";
	case PCI_VENDOR_ID_WEITEK:
		return "Weitek";
	case PCI_VENDOR_ID_CIRRUS:
		return "Cirrus Logic";
	case PCI_VENDOR_ID_BUSLOGIC:
		return "Bus Logic";
	case PCI_VENDOR_ID_N9:
		return "Number Nine";
	case PCI_VENDOR_ID_AI:
		return "Acer Incorporated";
	case PCI_VENDOR_ID_AL:
		return "Acer Labs";
	case PCI_VENDOR_ID_TSENG:
		return "Tseng'Lab";
	case PCI_VENDOR_ID_CMD:
		return "CMD";
	case PCI_VENDOR_ID_VISION:
		return "Vision";
	case PCI_VENDOR_ID_AMD:
		return "AMD";
	case PCI_VENDOR_ID_VLSI:
		return "VLSI";
	case PCI_VENDOR_ID_ADL:
		return "Advance Logic";
	case PCI_VENDOR_ID_SYMPHONY:
		return "Symphony";
	case PCI_VENDOR_ID_TRIDENT:
		return "Trident";
	case PCI_VENDOR_ID_CONTAQ:
		return "Contaq";
	case PCI_VENDOR_ID_NS:
		return "NS";
	case PCI_VENDOR_ID_VIA:
		return "VIA Technologies";
	case PCI_VENDOR_ID_SI:
		return "Silicon Integrated Systems";
	case PCI_VENDOR_ID_LEADTEK:
		return "Leadtek Research";
	case PCI_VENDOR_ID_IMS:
		return "IMS";
	case PCI_VENDOR_ID_ZEINET:
		return "ZeiNet";
	case PCI_VENDOR_ID_EF:
		return "Efficient Networks";
	case PCI_VENDOR_ID_HER:
		return "Hercules";
	case PCI_VENDOR_ID_ATRONICS:
		return "Atronics";
	case PCI_VENDOR_ID_CT:
		return "Chips & Technologies";
	case PCI_VENDOR_ID_FD:
		return "Future Domain";
	case PCI_VENDOR_ID_WINBOND:
		return "Winbond";
	case PCI_VENDOR_ID_3COM:
		return "3Com";
	case PCI_VENDOR_ID_PROMISE:
		return "Promise Technology";
	case PCI_VENDOR_ID_QLOGIC:
		return "Q Logic";
	case PCI_VENDOR_ID_FOREX:
		return "Forex";
	case PCI_VENDOR_ID_OLICOM:
		return "Olicom";
	case PCI_VENDOR_ID_SIERRA:
		return "Sierra";
	case PCI_VENDOR_ID_ACC:
		return "ACC MICROELECTRONICS";
	case PCI_VENDOR_ID_ASP:
		return "Advanced System Products";
	case PCI_VENDOR_ID_CERN:
		return "CERN";
	case PCI_VENDOR_ID_AMCC:
		return "AMCC";
	case PCI_VENDOR_ID_INTERG:
		return "Intergraphics";
	case PCI_VENDOR_ID_REALTEK:
		return "Realtek";
	case PCI_VENDOR_ID_INIT:
		return "Initio Corp";
	case PCI_VENDOR_ID_VORTEX:
		return "VORTEX";
	case PCI_VENDOR_ID_FORE:
		return "Fore Systems";
	case PCI_VENDOR_ID_IMAGINGTECH:
		return "Imaging Technology";
	case PCI_VENDOR_ID_PLX:
		return "PLX";
	case PCI_VENDOR_ID_ALLIANCE:
		return "Alliance";
	case PCI_VENDOR_ID_VMIC:
		return "VMIC";
	case PCI_VENDOR_ID_MUTECH:
		return "Mutech";
	case PCI_VENDOR_ID_TOSHIBA:
		return "Toshiba";
	case PCI_VENDOR_ID_SPECIALIX:
		return "Specialix";
	case PCI_VENDOR_ID_RP:
		return "Comtrol";
	case PCI_VENDOR_ID_CYCLADES:
		return "Cyclades";
	case PCI_VENDOR_ID_TEKRAM:
		return "Tekram";
	case PCI_VENDOR_ID_AVANCE:
		return "Avance";
	default:
		return "Unknown vendor";
		/* need more information */
	}
}
