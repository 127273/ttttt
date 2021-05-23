#pragma once

#include <stdint.h>
//#include <vector.h>
#include <driver.h>

#define PCI_BIST_CAPABLE (1 << 7)
#define PCI_BIST_START (1 << 6)
#define PCI_BIST_COMPLETION_CODE (0xF)

#define PCI_CMD_INTERRUPT_DISABLE (1 << 10)
#define PCI_CMD_FAST_BTB_ENABLE (1 << 9)
#define PCI_CMD_SERR_ENABLE (1 << 8)
#define PCI_CMD_PARITY_ERROR_RESPONSE (1 << 6)
#define PCI_CMD_VGA_PALETTE_SNOOP (1 << 5)
#define PCI_CMD_MEMORY_WRITE_INVALIDATE_ENABLE (1 << 4)
#define PCI_CMD_SPECIAL_CYCLES (1 << 3)
#define PCI_CMD_BUS_MASTER (1 << 2)
#define PCI_CMD_MEMORY_SPACE (1 << 1)
#define PCI_CMD_IO_SPACE (1 << 0)

#define PCI_STATUS_CAPABILITIES (1 << 4)

#define PCI_CLASS_UNCLASSIFIED 0x0
#define PCI_CLASS_STORAGE 0x1
#define PCI_CLASS_NETWORK 0x2
#define PCI_CLASS_DISPLAY 0x3
#define PCI_CLASS_MULTIMEDIA 0x4
#define PCI_CLASS_MEMORY 0x5
#define PCI_CLASS_BRIDGE 0x6
#define PCI_CLASS_COMMUNICATION 0x7
#define PCI_CLASS_PERIPHERAL 0x8
#define PCI_CLASS_INPUT_DEVICE 0x9
#define PCI_CLASS_DOCKING_STATION 0xA
#define PCI_CLASS_PROCESSOR 0xB
#define PCI_CLASS_SERIAL_BUS 0xC
#define PCI_CLASS_WIRELESS_CONTROLLER 0xD
#define PCI_CLASS_INTELLIGENT_CONTROLLER 0xE
#define PCI_CLASS_SATELLITE_COMMUNICATION 0xF
#define PCI_CLASS_ENCRYPTON 0x10
#define PCI_CLASS_SIGNAL_PROCESSING 0x11

#define PCI_CLASS_COPROCESSOR 0x40

#define PCI_SUBCLASS_IDE 0x1
#define PCI_SUBCLASS_FLOPPY 0x2
#define PCI_SUBCLASS_ATA 0x5
#define PCI_SUBCLASS_SATA 0x6
#define PCI_SUBCLASS_NVM 0x8

#define PCI_SUBCLASS_ETHERNET 0x0

#define PCI_SUBCLASS_VGA_COMPATIBLE 0x0
#define PCI_SUBCLASS_XGA 0x1

#define PCI_SUBCLASS_USB 0x3

#define PCI_PROGIF_UHCI 0x20
#define PCI_PROGIF_OHCI 0x10
#define PCI_PROGIF_EHCI 0x20
#define PCI_PROGIF_XHCI 0x30

#define PCI_IO_PORT_CONFIG_ADDRESS 0xCF8
#define PCI_IO_PORT_CONFIG_DATA 0xCFC

#define PCI_CAP_MSI_ADDRESS_BASE 0xFEE00000
#define PCI_CAP_MSI_CONTROL_64 (1 << 7)				// 64-bit address capable
#define PCI_CAP_MSI_CONTROL_VECTOR_MASKING (1 << 8) // Enable Vector Masking
#define PCI_CAP_MSI_CONTROL_MME_MASK (0x7U << 4)
#define PCI_CAP_MSI_CONTROL_SET_MME(x) ((x & 0x7) << 4) // Multiple message enable
#define PCI_CAP_MSI_CONTROL_MMC(x) ((x >> 1) & 0x7)		// Multiple Message Capable
#define PCI_CAP_MSI_CONTROL_ENABLE (1 << 0)				// MSI Enable

#define PCI_VENDOR_ID 0x00			 /* 16 bits */
#define PCI_DEVICE_ID 0x02			 /* 16 bits */
#define PCI_COMMAND 0x04			 /* 16 bits */
#define PCI_COMMAND_IO 0x1			 /* Enable response in I/O space */
#define PCI_COMMAND_MEMORY 0x2		 /* Enable response in I/O space */
#define PCI_COMMAND_MASTER 0x4		 /* Enable bus mastering */
#define PCI_COMMAND_SPECIAL 0x8		 /* Enable response to special cycles */
#define PCI_COMMAND_INVALIDATE 0x10	 /* Use memory write and invalidate */
#define PCI_COMMAND_VGA_PALETTE 0x20 /* Enable palette snooping */
#define PCI_COMMAND_PARITY 0x40		 /* Enable parity checking */
#define PCI_COMMAND_WAIT 0x80		 /* Enable address/data stepping */
#define PCI_COMMAND_SERR 0x100		 /* Enable SERR */
#define PCI_COMMAND_FAST_BACK 0x200	 /* Enable back-to-back writes */

#define PCI_STATUS 0x06				 /* 16 bits */
#define PCI_STATUS_FAST_BACK 0x80	 /* Accept fast-back to back */
#define PCI_STATUS_PARITY 0x100		 /* Detected parity error */
#define PCI_STATUS_DEVSEL_MASK 0x600 /* DEVSEL timing */
#define PCI_STATUS_DEVSEL_FAST 0x000
#define PCI_STATUS_DEVSEL_MEDIUM 0x200
#define PCI_STATUS_DEVSEL_SLOW 0x400
#define PCI_STATUS_SIG_TARGET_ABORT 0x800  /* Set on target abort */
#define PCI_STATUS_REC_TARGET_ABORT 0x1000 /* Master ack of " */
#define PCI_STATUS_REC_MASTER_ABORT 0x2000 /* Set on master abort */
#define PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 /* Set when we drive SERR */
#define PCI_STATUS_DETECTED_PARITY 0x8000  /* Set on parity error */

#define PCI_CLASS_REVISION 0x08 /* High 24 bits are class, low 8 \
				   revision */
#define PCI_REVISION_ID 0x08	/* Revision ID */
#define PCI_CLASS_PROG 0x09		/* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE 0x0a	/* Device class */

#define PCI_CACHE_LINE_SIZE 0x0c /* 8 bits */
#define PCI_LATENCY_TIMER 0x0d	 /* 8 bits */
#define PCI_HEADER_TYPE 0x0e	 /* 8 bits */
#define PCI_BIST 0x0f			 /* 8 bits */
#define PCI_BIST_CODE_MASK 0x0f	 /* Return result */

/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of 
 * 0xffffffff to the register, and reading it back.  Only 
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0 0x10		/* 32 bits */
#define PCI_BASE_ADDRESS_1 0x14		/* 32 bits */
#define PCI_BASE_ADDRESS_2 0x18		/* 32 bits */
#define PCI_BASE_ADDRESS_3 0x1c		/* 32 bits */
#define PCI_BASE_ADDRESS_4 0x20		/* 32 bits */
#define PCI_BASE_ADDRESS_5 0x24		/* 32 bits */
#define PCI_BASE_ADDRESS_SPACE 0x01 /* 0 = memory, 1 = I/O */
#define PCI_BASE_ADDRESS_SPACE_IO 0x01
#define PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06
#define PCI_BASE_ADDRESS_MEM_TYPE_32 0x00  /* 32 bit address */
#define PCI_BASE_ADDRESS_MEM_TYPE_1M 0x02  /* Below 1M */
#define PCI_BASE_ADDRESS_MEM_TYPE_64 0x04  /* 64 bit address */
#define PCI_BASE_ADDRESS_MEM_PREFETCH 0x08 /* prefetchable? */
#define PCI_BASE_ADDRESS_MEM_MASK (~0x0f)
#define PCI_BASE_ADDRESS_IO_MASK (~0x03)
/* bit 1 is reserved if address_space = 1 */

/* 0x28-0x2f are reserved */
#define PCI_ROM_ADDRESS 0x30		/* 32 bits */
#define PCI_ROM_ADDRESS_ENABLE 0x01 /* Write 1 to enable ROM, \
				   bits 31..11 are address,                   \
				   10..2 are reserved */
/* 0x34-0x3b are reserved */

#define PCI_INTERRUPT_PIN 0x3d /* 8 bits */
#define PCI_MIN_GNT 0x3e	   /* 8 bits */
#define PCI_MAX_LAT 0x3f	   /* 8 bits */

#define PCI_CLASS_NOT_DEFINED 0x0000
#define PCI_CLASS_NOT_DEFINED_VGA 0x0001

#define PCI_BASE_CLASS_STORAGE 0x01
#define PCI_CLASS_STORAGE_SCSI 0x0100
#define PCI_CLASS_STORAGE_IDE 0x0101
#define PCI_CLASS_STORAGE_FLOPPY 0x0102
#define PCI_CLASS_STORAGE_IPI 0x0103
#define PCI_CLASS_STORAGE_OTHER 0x0180

#define PCI_BASE_CLASS_NETWORK 0x02
#define PCI_CLASS_NETWORK_ETHERNET 0x0200
#define PCI_CLASS_NETWORK_TOKEN_RING 0x0201
#define PCI_CLASS_NETWORK_FDDI 0x0202
#define PCI_CLASS_NETWORK_OTHER 0x0280

#define PCI_BASE_CLASS_DISPLAY 0x03
#define PCI_CLASS_DISPLAY_VGA 0x0300
#define PCI_CLASS_DISPLAY_XGA 0x0301
#define PCI_CLASS_DISPLAY_OTHER 0x0380

#define PCI_BASE_CLASS_MULTIMEDIA 0x04
#define PCI_CLASS_MULTIMEDIA_VIDEO 0x0400
#define PCI_CLASS_MULTIMEDIA_AUDIO 0x0401
#define PCI_CLASS_MULTIMEDIA_OTHER 0x0480

#define PCI_BASE_CLASS_MEMORY 0x05
#define PCI_CLASS_MEMORY_RAM 0x0500
#define PCI_CLASS_MEMORY_FLASH 0x0501
#define PCI_CLASS_MEMORY_OTHER 0x0580

#define PCI_BASE_CLASS_BRIDGE 0x06
#define PCI_CLASS_BRIDGE_HOST 0x0600
#define PCI_CLASS_BRIDGE_ISA 0x0601
#define PCI_CLASS_BRIDGE_EISA 0x0602
#define PCI_CLASS_BRIDGE_MC 0x0603
#define PCI_CLASS_BRIDGE_PCI 0x0604
#define PCI_CLASS_BRIDGE_PCMCIA 0x0605
#define PCI_CLASS_BRIDGE_OTHER 0x0680

#define PCI_CLASS_OTHERS 0xff

#define PCI_BASE_CLASS_COMMUNICATION 0x07
#define PCI_CLASS_COMMUNICATION_SERIAL 0x0700
#define PCI_CLASS_COMMUNICATION_PARALLEL 0x0701
#define PCI_CLASS_COMMUNICATION_OTHER 0x0780

#define PCI_BASE_CLASS_SYSTEM 0x08
#define PCI_CLASS_SYSTEM_PIC 0x0800
#define PCI_CLASS_SYSTEM_DMA 0x0801
#define PCI_CLASS_SYSTEM_TIMER 0x0802
#define PCI_CLASS_SYSTEM_RTC 0x0803
#define PCI_CLASS_SYSTEM_OTHER 0x0880

#define PCI_BASE_CLASS_INPUT 0x09
#define PCI_CLASS_INPUT_KEYBOARD 0x0900
#define PCI_CLASS_INPUT_PEN 0x0901
#define PCI_CLASS_INPUT_MOUSE 0x0902
#define PCI_CLASS_INPUT_OTHER 0x0980

#define PCI_BASE_CLASS_DOCKING 0x0a
#define PCI_CLASS_DOCKING_GENERIC 0x0a00
#define PCI_CLASS_DOCKING_OTHER 0x0a01

#define PCI_BASE_CLASS_PROCESSOR 0x0b
#define PCI_CLASS_PROCESSOR_386 0x0b00
#define PCI_CLASS_PROCESSOR_486 0x0b01
#define PCI_CLASS_PROCESSOR_PENTIUM 0x0b02
#define PCI_CLASS_PROCESSOR_ALPHA 0x0b10
#define PCI_CLASS_PROCESSOR_POWERPC 0x0b20
#define PCI_CLASS_PROCESSOR_CO 0x0b40

#define PCI_BASE_CLASS_SERIAL 0x0c
#define PCI_CLASS_SERIAL_FIREWIRE 0x0c00
#define PCI_CLASS_SERIAL_ACCESS 0x0c01
#define PCI_CLASS_SERIAL_SSA 0x0c02
#define PCI_CLASS_SERIAL_USB 0x0c03
#define PCI_CLASS_SERIAL_FIBER 0x0c04

#define PCI_CLASS_OTHERS 0xff
#define PCI_CLASS_BRIDGE_NUBUS 0x0606
#define PCI_CLASS_BRIDGE_CARDBUS 0x0607
#define PCI_CLASS_NETWORK_ATM 0x0203
#define PCI_CLASS_STORAGE_RAID 0x0104

#define PCI_VENDOR_ID_NCR 0x1000
#define PCI_DEVICE_ID_NCR_53C810 0x0001
#define PCI_DEVICE_ID_NCR_53C815 0x0004
#define PCI_DEVICE_ID_NCR_53C820 0x0002
#define PCI_DEVICE_ID_NCR_53C825 0x0003

#define PCI_VENDOR_ID_ADAPTEC 0x9004
#define PCI_DEVICE_ID_ADAPTEC_2940 0x7178
#define PCI_DEVICE_ID_ADAPTEC_294x 0x7078

#define PCI_VENDOR_ID_DPT 0x1044
#define PCI_DEVICE_ID_DPT 0xa400

#define PCI_VENDOR_ID_S3 0x5333
#define PCI_DEVICE_ID_S3_864_1 0x88c0
#define PCI_DEVICE_ID_S3_864_2 0x88c1
#define PCI_DEVICE_ID_S3_868 0x8880
#define PCI_DEVICE_ID_S3_928 0x88b0
#define PCI_DEVICE_ID_S3_964_1 0x88d0
#define PCI_DEVICE_ID_S3_964_2 0x88d1
#define PCI_DEVICE_ID_S3_811 0x8811
#define PCI_DEVICE_ID_S3_968 0x88f0

#define PCI_VENDOR_ID_OPTI 0x1045
#define PCI_DEVICE_ID_OPTI_82C822 0xc822
#define PCI_DEVICE_ID_OPTI_82C621 0xc621
#define PCI_DEVICE_ID_OPTI_82C557 0xc557
#define PCI_DEVICE_ID_OPTI_82C558 0xc558

#define PCI_VENDOR_ID_UMC 0x1060
#define PCI_DEVICE_ID_UMC_UM8881F 0x8881
#define PCI_DEVICE_ID_UMC_UM8891A 0x0891
#define PCI_DEVICE_ID_UMC_UM8886F 0x8886
#define PCI_DEVICE_ID_UMC_UM8673F 0x0101

#define PCI_VENDOR_ID_DEC 0x1011
#define PCI_DEVICE_ID_DEC_TULIP 0x0002
#define PCI_DEVICE_ID_DEC_TULIP_FAST 0x0009
#define PCI_DEVICE_ID_DEC_FDDI 0x000F
#define PCI_DEVICE_ID_DEC_BRD 0x0001

#define PCI_VENDOR_ID_MATROX 0x102B
#define PCI_DEVICE_ID_MATROX_MGA_2 0x0518
#define PCI_DEVICE_ID_MATROX_MGA_IMP 0x0d10

#define PCI_VENDOR_ID_INTEL 0x8086
#define PCI_DEVICE_ID_INTEL_82378 0x0484
#define PCI_DEVICE_ID_INTEL_82424 0x0483
#define PCI_DEVICE_ID_INTEL_82375 0x0482
#define PCI_DEVICE_ID_INTEL_82434 0x04a3
#define PCI_DEVICE_ID_INTEL_82430 0x0486
#define PCI_DEVICE_ID_INTEL_82437 0x122d
#define PCI_DEVICE_ID_INTEL_82371 0x122e

#define PCI_VENDOR_ID_SMC 0x1042
#define PCI_DEVICE_ID_SMC_37C665 0x1000

#define PCI_VENDOR_ID_ATI 0x1002
#define PCI_DEVICE_ID_ATI_M32 0x4158
#define PCI_DEVICE_ID_ATI_M64 0x4758

#define PCI_VENDOR_ID_WEITEK 0x100e
#define PCI_DEVICE_ID_WEITEK_P9000 0x9001
#define PCI_DEVICE_ID_WEITEK_P9100 0x9100

#define PCI_VENDOR_ID_CIRRUS 0x1013
#define PCI_DEVICE_ID_CIRRUS_5430 0x00A0
#define PCI_DEVICE_ID_CIRRUS_5434_4 0x00A4
#define PCI_DEVICE_ID_CIRRUS_5434_8 0x00A8
#define PCI_DEVICE_ID_CIRRUS_6729 0x1100

#define PCI_VENDOR_ID_BUSLOGIC 0x104B
#define PCI_DEVICE_ID_BUSLOGIC_946C 0x1040
#define PCI_DEVICE_ID_BUSLOGIC_946C_2 0x0140

#define PCI_VENDOR_ID_N9 0x105D
#define PCI_DEVICE_ID_N9_I128 0x2309

#define PCI_VENDOR_ID_AI 0x1025
#define PCI_DEVICE_ID_AI_M1435 0x1435

#define PCI_VENDOR_ID_AL 0x10b9
#define PCI_DEVICE_ID_AL_M1445 0x1445
#define PCI_DEVICE_ID_AL_M1449 0x1449
#define PCI_DEVICE_ID_AL_M1451 0x1451
#define PCI_DEVICE_ID_AL_M4803 0x5215

#define PCI_VENDOR_ID_TSENG 0x100c
#define PCI_DEVICE_ID_TSENG_W32P_2 0x3202
#define PCI_DEVICE_ID_TSENG_W32P_b 0x3205
#define PCI_DEVICE_ID_TSENG_W32P_c 0x3206
#define PCI_DEVICE_ID_TSENG_W32P_d 0x3207

#define PCI_VENDOR_ID_CMD 0x1095
#define PCI_DEVICE_ID_CMD_640 0x0640

#define PCI_VENDOR_ID_VISION 0x1098
#define PCI_DEVICE_ID_VISION_QD8500 0x0001
#define PCI_DEVICE_ID_VISION_QD8580 0x0002

#define PCI_VENDOR_ID_AMD 0x1022
#define PCI_DEVICE_ID_AMD_LANCE 0x2000
#define PCI_DEVICE_ID_AMD_SCSI 0x2020

#define PCI_VENDOR_ID_VLSI 0x1004
#define PCI_DEVICE_ID_VLSI_82C593 0x0006
#define PCI_DEVICE_ID_VLSI_82C592 0x0005

#define PCI_VENDOR_ID_ADL 0x1005
#define PCI_DEVICE_ID_ADL_2301 0x2301

#define PCI_VENDOR_ID_SYMPHONY 0x1c1c
#define PCI_DEVICE_ID_SYMPHONY_101 0x0001

#define PCI_VENDOR_ID_TRIDENT 0x1023
#define PCI_DEVICE_ID_TRIDENT_9420 0x9420
#define PCI_DEVICE_ID_TRIDENT_9440 0x9440

#define PCI_VENDOR_ID_CONTAQ 0x1080
#define PCI_DEVICE_ID_CONTAQ_82C599 0x0600

#define PCI_VENDOR_ID_NS 0x100b
#define PCI_DEVICE_ID_NS_87410 0xd001

#define PCI_VENDOR_ID_VIA 0x1106
#define PCI_DEVICE_ID_VIA_82C505 0x0505
#define PCI_DEVICE_ID_VIA_82C576 0x0576
#define PCI_DEVICE_ID_VIA_82C561 0x0561

#define PCI_VENDOR_ID_SI 0x1039
#define PCI_DEVICE_ID_SI_496 0x0496
#define PCI_DEVICE_ID_SI_501 0x0406
#define PCI_DEVICE_ID_SI_503 0x0008
#define PCI_DEVICE_ID_SI_601 0x0601

#define PCI_VENDOR_ID_LEADTEK 0x107d
#define PCI_DEVICE_ID_LEADTEK_805 0x0000

#define PCI_VENDOR_ID_IMS 0x10e0
#define PCI_DEVICE_ID_IMS_8849 0x8849

#define PCI_VENDOR_ID_ZEINET 0x1193
#define PCI_DEVICE_ID_ZEINET_1221 0x0001

#define PCI_VENDOR_ID_EF 0x111a
#define PCI_DEVICE_ID_EF_ATM 0x0000

#define PCI_VENDOR_ID_HER 0xedd8
#define PCI_DEVICE_ID_HER_STING 0xa091

#define PCI_VENDOR_ID_ATRONICS 0x907f
#define PCI_DEVICE_ID_ATRONICS_2015 0x2015

#define PCI_VENDOR_ID_CT 0x102c
#define PCI_DEVICE_ID_CT_65545 0x00d8

#define PCI_VENDOR_ID_FD 0x1036
#define PCI_DEVICE_ID_FD_36C70 0x0000

#define PCI_VENDOR_ID_WINBOND 0x10ad
#define PCI_DEVICE_ID_WINBOND_83769 0x0001

#define PCI_VENDOR_ID_3COM 0x10b7
#define PCI_DEVICE_ID_3COM_3C590 0x5900
#define PCI_DEVICE_ID_3COM_3C595TX 0x5950
#define PCI_DEVICE_ID_3COM_3C595T4 0x5951
#define PCI_DEVICE_ID_3COM_3C595MII 0x5952

#define PCI_VENDOR_ID_PROMISE 0x105a
#define PCI_DEVICE_ID_PROMISE_5300 0x5300

#define PCI_VENDOR_ID_QLOGIC 0x1077
#define PCI_DEVICE_ID_QLOGIC_ISP1020 0x1020
#define PCI_DEVICE_ID_QLOGIC_ISP1022 0x1022

#define PCI_VENDOR_ID_X 0x1061
#define PCI_DEVICE_ID_X_AGX016 0x0001

#define PCI_VENDOR_ID_LEADTEK 0x107d
#define PCI_DEVICE_ID_LEADTEK_805 0x0000

#define PCI_DEVICE_ID_CONTAQ_82C599 0x0600

#define PCI_VENDOR_ID_FOREX 0x1083

#define PCI_VENDOR_ID_OLICOM 0x108d

#define PCI_VENDOR_ID_CMD 0x1095
#define PCI_DEVICE_ID_CMD_640 0x0640
#define PCI_DEVICE_ID_CMD_646 0x0646

#define PCI_VENDOR_ID_VISION 0x1098
#define PCI_DEVICE_ID_VISION_QD8500 0x0001
#define PCI_DEVICE_ID_VISION_QD8580 0x0002

#define PCI_VENDOR_ID_SIERRA 0x10a8
#define PCI_DEVICE_ID_SIERRA_STB 0x0000

#define PCI_VENDOR_ID_ACC 0x10aa
#define PCI_DEVICE_ID_ACC_2056 0x0000

#define PCI_VENDOR_ID_WINBOND 0x10ad
#define PCI_DEVICE_ID_WINBOND_83769 0x0001
#define PCI_DEVICE_ID_WINBOND_82C105 0x0105

#define PCI_VENDOR_ID_3COM 0x10b7
#define PCI_DEVICE_ID_3COM_3C590 0x5900
#define PCI_DEVICE_ID_3COM_3C595TX 0x5950
#define PCI_DEVICE_ID_3COM_3C595T4 0x5951
#define PCI_DEVICE_ID_3COM_3C595MII 0x5952

#define PCI_VENDOR_ID_AL 0x10b9
#define PCI_DEVICE_ID_AL_M1445 0x1445
#define PCI_DEVICE_ID_AL_M1449 0x1449
#define PCI_DEVICE_ID_AL_M1451 0x1451
#define PCI_DEVICE_ID_AL_M1461 0x1461
#define PCI_DEVICE_ID_AL_M1489 0x1489
#define PCI_DEVICE_ID_AL_M1511 0x1511
#define PCI_DEVICE_ID_AL_M1513 0x1513
#define PCI_DEVICE_ID_AL_M4803 0x5215

#define PCI_VENDOR_ID_ASP 0x10cd
#define PCI_DEVICE_ID_ASP_ABP940 0x1200

#define PCI_VENDOR_ID_CERN 0x10dc
#define PCI_DEVICE_ID_CERN_SPSB_PMC 0x0001
#define PCI_DEVICE_ID_CERN_SPSB_PCI 0x0002

#define PCI_VENDOR_ID_IMS 0x10e0
#define PCI_DEVICE_ID_IMS_8849 0x8849

#define PCI_VENDOR_ID_TEKRAM2 0x10e1
#define PCI_DEVICE_ID_TEKRAM2_690c 0x690c

#define PCI_VENDOR_ID_AMCC 0x10e8
#define PCI_DEVICE_ID_AMCC_MYRINET 0x8043

#define PCI_VENDOR_ID_INTERG 0x10ea
#define PCI_DEVICE_ID_INTERG_1680 0x1680

#define PCI_VENDOR_ID_REALTEK 0x10ec
#define PCI_DEVICE_ID_REALTEK_8029 0x8029

#define PCI_VENDOR_ID_INIT 0x1101
#define PCI_DEVICE_ID_INIT_320P 0x9100

#define PCI_VENDOR_ID_VIA 0x1106
#define PCI_DEVICE_ID_VIA_82C505 0x0505
#define PCI_DEVICE_ID_VIA_82C561 0x0561
#define PCI_DEVICE_ID_VIA_82C576 0x0576
#define PCI_DEVICE_ID_VIA_82C416 0x1571

#define PCI_VENDOR_ID_VORTEX 0x1119
#define PCI_DEVICE_ID_VORTEX_GDT60x0 0x0000
#define PCI_DEVICE_ID_VORTEX_GDT6000B 0x0001
#define PCI_DEVICE_ID_VORTEX_GDT6x10 0x0002
#define PCI_DEVICE_ID_VORTEX_GDT6x20 0x0003
#define PCI_DEVICE_ID_VORTEX_GDT6530 0x0004
#define PCI_DEVICE_ID_VORTEX_GDT6550 0x0005
#define PCI_DEVICE_ID_VORTEX_GDT6x17 0x0006
#define PCI_DEVICE_ID_VORTEX_GDT6x27 0x0007
#define PCI_DEVICE_ID_VORTEX_GDT6537 0x0008
#define PCI_DEVICE_ID_VORTEX_GDT6557 0x0009
#define PCI_DEVICE_ID_VORTEX_GDT6x15 0x000a
#define PCI_DEVICE_ID_VORTEX_GDT6x25 0x000b
#define PCI_DEVICE_ID_VORTEX_GDT6535 0x000c
#define PCI_DEVICE_ID_VORTEX_GDT6555 0x000d

#define PCI_VENDOR_ID_EF 0x111a
#define PCI_DEVICE_ID_EF_ATM_FPGA 0x0000
#define PCI_DEVICE_ID_EF_ATM_ASIC 0x0002

#define PCI_VENDOR_ID_FORE 0x1127
#define PCI_DEVICE_ID_FORE_PCA200PC 0x0210
#define PCI_DEVICE_ID_FORE_PCA200E 0x0300

#define PCI_VENDOR_ID_IMAGINGTECH 0x112f
#define PCI_DEVICE_ID_IMAGINGTECH_ICPCI 0x0000

#define PCI_VENDOR_ID_PLX 0x113c
#define PCI_DEVICE_ID_PLX_9060 0x0001

#define PCI_VENDOR_ID_ALLIANCE 0x1142
#define PCI_DEVICE_ID_ALLIANCE_PROMOTIO 0x3210
#define PCI_DEVICE_ID_ALLIANCE_PROVIDEO 0x6422

#define PCI_VENDOR_ID_VMIC 0x114a
#define PCI_DEVICE_ID_VMIC_VME 0x7587

#define PCI_VENDOR_ID_DIGI 0x114f
#define PCI_DEVICE_ID_DIGI_RIGHTSWITCH 0x0003

#define PCI_VENDOR_ID_MUTECH 0x1159
#define PCI_DEVICE_ID_MUTECH_MV1000 0x0001

#define PCI_VENDOR_ID_TOSHIBA 0x1179

#define PCI_VENDOR_ID_ZEITNET 0x1193
#define PCI_DEVICE_ID_ZEITNET_1221 0x0001
#define PCI_DEVICE_ID_ZEITNET_1225 0x0002

#define PCI_VENDOR_ID_SPECIALIX 0x11cb
#define PCI_DEVICE_ID_SPECIALIX_XIO 0x4000
#define PCI_DEVICE_ID_SPECIALIX_RIO 0x8000

#define PCI_VENDOR_ID_RP 0x11fe
#define PCI_DEVICE_ID_RP8OCTA 0x0001
#define PCI_DEVICE_ID_RP8INTF 0x0002
#define PCI_DEVICE_ID_RP16INTF 0x0003
#define PCI_DEVICE_ID_RP32INTF 0x0004

#define PCI_VENDOR_ID_CYCLADES 0x120e
#define PCI_DEVICE_ID_CYCLOM_Y_Lo 0x0100
#define PCI_DEVICE_ID_CYCLOM_Y_Hi 0x0101
#define PCI_DEVICE_ID_CYCLOM_Z_Lo 0x0200
#define PCI_DEVICE_ID_CYCLOM_Z_Hi 0x0201

#define PCI_VENDOR_ID_SYMPHONY 0x1c1c
#define PCI_DEVICE_ID_SYMPHONY_101 0x0001

#define PCI_VENDOR_ID_TEKRAM 0x1de1
#define PCI_DEVICE_ID_TEKRAM_DC290 0xdc29

#define PCI_VENDOR_ID_AVANCE 0x4005
#define PCI_DEVICE_ID_AVANCE_2302 0x2302

#define PCI_VENDOR_ID_S3 0x5333
#define PCI_DEVICE_ID_S3_811 0x8811
#define PCI_DEVICE_ID_S3_868 0x8880
#define PCI_DEVICE_ID_S3_928 0x88b0
#define PCI_DEVICE_ID_S3_864_1 0x88c0
#define PCI_DEVICE_ID_S3_864_2 0x88c1
#define PCI_DEVICE_ID_S3_964_1 0x88d0
#define PCI_DEVICE_ID_S3_964_2 0x88d1
#define PCI_DEVICE_ID_S3_968 0x88f0

#define PCI_VENDOR_ID_INTEL 0x8086
#define PCI_DEVICE_ID_INTEL_82375 0x0482
#define PCI_DEVICE_ID_INTEL_82424 0x0483
#define PCI_DEVICE_ID_INTEL_82378 0x0484
#define PCI_DEVICE_ID_INTEL_82430 0x0486
#define PCI_DEVICE_ID_INTEL_82434 0x04a3
#define PCI_DEVICE_ID_INTEL_7116 0x1223
#define PCI_DEVICE_ID_INTEL_82596 0x1226
#define PCI_DEVICE_ID_INTEL_82865 0x1227
#define PCI_DEVICE_ID_INTEL_82557 0x1229
#define PCI_DEVICE_ID_INTEL_82437 0x122d
#define PCI_DEVICE_ID_INTEL_82371_0 0x122e
#define PCI_DEVICE_ID_INTEL_82371_1 0x1230
#define PCI_DEVICE_ID_INTEL_82439 0x1250
#define PCI_DEVICE_ID_INTEL_82371SB_0 0x7000
#define PCI_DEVICE_ID_INTEL_82371SB_1 0x7010
#define PCI_DEVICE_ID_INTEL_P6 0x84c4

#define PCI_VENDOR_ID_ADAPTEC 0x9004
#define PCI_DEVICE_ID_ADAPTEC_7850 0x5078
#define PCI_DEVICE_ID_ADAPTEC_7855 0x5578
#define PCI_DEVICE_ID_ADAPTEC_7870 0x7078
#define PCI_DEVICE_ID_ADAPTEC_7871 0x7178
#define PCI_DEVICE_ID_ADAPTEC_7872 0x7278
#define PCI_DEVICE_ID_ADAPTEC_7873 0x7378
#define PCI_DEVICE_ID_ADAPTEC_7874 0x7478
#define PCI_DEVICE_ID_ADAPTEC_7880 0x8078
#define PCI_DEVICE_ID_ADAPTEC_7881 0x8178
#define PCI_DEVICE_ID_ADAPTEC_7882 0x8278
#define PCI_DEVICE_ID_ADAPTEC_7883 0x8378
#define PCI_DEVICE_ID_ADAPTEC_7884 0x8478

#define PCI_VENDOR_ID_ATRONICS 0x907f
#define PCI_DEVICE_ID_ATRONICS_2015 0x2015

#define PCI_VENDOR_ID_HER 0xedd8
#define PCI_DEVICE_ID_HER_STING 0xa091
#define PCI_DEVICE_ID_HER_STINGARK 0xa099

struct pci_devs
{
	struct pci_bus *bus;	  /* bus this device is on */
	struct pci_devs *sibling; /* next device on this bus */
	struct pci_devs *next;	  /* chain of all devices */

	void *sysdata; /* hook for sys-specific extension */

	unsigned int devfn; /* encoded device & function index */
	unsigned short vendor;
	unsigned short device;
	unsigned int strclass;	 /* 3 bytes: (base,sub,prog-if) */
	unsigned int master : 1; /* set if device is master capable */
	/*
	 * In theory, the irq level can be read from configuration
	 * space and all would be fine.  However, old PCI chips don't
	 * support these registers and return 0 instead.  For example,
	 * the Vision864-P rev 0 chip can uses INTA, but returns 0 in
	 * the interrupt line and pin registers.  pci_init()
	 * initializes this field with the value at PCI_INTERRUPT_LINE
	 * and it is the job of pcibios_fixup() to change it if
	 * necessary.  The field must not be 0 unless the device
	 * cannot generate interrupts at all.
	 */
	unsigned char irq; /* irq generated by this device */
};

struct pci_bus
{
	struct pci_bus *parent;	  /* parent bus this bridge is on */
	struct pci_bus *children; /* chain of P2P bridges on this bus */
	struct pci_bus *next;	  /* chain of all PCI buses */

	struct pci_devs *self;	  /* bridge device as seen by parent */
	struct pci_devs *devices; /* devices behind this bridge */

	void *sysdata; /* hook for sys-specific extension */

	unsigned char number;	   /* bus number */
	unsigned char primary;	   /* number of primary bridge */
	unsigned char secondary;   /* number of secondary bridge */
	unsigned char subordinate; /* max number of subordinate buses */
};

/*
 * This is used to map a vendor-id/device-id pair into device-specific
 * information.
 */
struct pci_dev_info
{
	unsigned short vendor; /* vendor id */
	unsigned short device; /* device id */

	const char *name;		   /* device name */
	unsigned char bridge_type; /* bridge type or 0xff */
};

struct pci_dev_info *pci_lookup_dev(unsigned int vendor, unsigned int dev);
const char *pci_strdev(unsigned int vendor, unsigned int device);
const char *pci_strvendor(unsigned int vendor);
const char *pci_strclass(unsigned int strclass);
char *pci_strbioserr(int error);
extern struct pci_dev_info dev_info[];

/* bios error */
#define PCIBIOS_SUCCESSFUL 0x00
#define PCIBIOS_FUNC_NOT_SUPPORTED 0x81
#define PCIBIOS_BAD_VENDOR_ID 0x83
#define PCIBIOS_DEVICE_NOT_FOUND 0x86
#define PCIBIOS_BAD_REGISTER_NUMBER 0x87
#define PCIBIOS_SET_FAILED 0x88
#define PCIBIOS_BUFFER_TOO_SMALL 0x89

typedef union pci_dev
{
	uint32_t bits;
	struct
	{
		uint32_t always_zero : 2;
		uint32_t field_num : 6;
		uint32_t function_num : 3;
		uint32_t device_num : 5;
		uint32_t bus_num : 8;
		uint32_t reserved : 7;
		uint32_t enable : 1;
	};
} pci_dev_t;

// Ports
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

// Config Address Register

// Offset
#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08
#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0a
#define PCI_CLASS 0x0b
#define PCI_CACHE_LINE_SIZE 0x0c
#define PCI_LATENCY_TIMER 0x0d
#define PCI_HEADER_TYPE 0x0e
#define PCI_BIST 0x0f
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_INTERRUPT_LINE 0x3C
#define PCI_SECONDARY_BUS 0x09

// Device type
#define PCI_HEADER_TYPE_DEVICE 0
#define PCI_HEADER_TYPE_BRIDGE 1
#define PCI_HEADER_TYPE_CARDBUS 2
#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA 0x0106
#define PCI_NONE 0xFFFF

#define DEVICE_PER_BUS 32
#define FUNCTION_PER_DEVICE 32

uint32_t pci_read(pci_dev_t dev, uint32_t field);
void pci_write(pci_dev_t dev, uint32_t field, uint32_t value);
uint32_t get_device_type(pci_dev_t dev);
uint32_t get_secondary_bus(pci_dev_t dev);
uint32_t pci_reach_end(pci_dev_t dev);
pci_dev_t pci_scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, uint32_t device_type);
pci_dev_t pci_scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t device_type);
pci_dev_t pci_scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device_type);
pci_dev_t pci_get_device(uint16_t vendor_id, uint16_t device_id, uint32_t device_type);

void init_kernel_pci();

#include <memory.h>
#include <driver.h>
#include <port.h>
#include <isr.h>

enum
{
	MemoryMapping,
	InputOutput,

};
static inline uint8_t mmio_read8(uint64_t address)
{
	return *((volatile uint8_t *)(address));
}

static inline uint16_t mmio_read16(uint64_t address)
{
	return *((volatile uint16_t *)(address));
}

static inline uint32_t mmio_read32(uint64_t address)
{
	return *((volatile uint32_t *)(address));
}

static inline uint64_t mmio_read64(uint64_t address)
{
	return *((volatile uint64_t *)(address));
}

static inline void mmio_write8(uint64_t address, uint8_t value)
{
	(*((volatile uint8_t *)(address))) = value;
}

static inline void mmio_write16(uint64_t address, uint16_t value)
{
	(*((volatile uint16_t *)(address))) = value;
}

static inline void mmio_write32(uint64_t address, uint32_t value)
{
	(*((volatile uint32_t *)(address))) = value;
}

static inline void mmio_write64(uint64_t address, uint64_t value)
{
	(*((volatile uint64_t *)(address))) = value;
}

namespace myos
{
	namespace PCI
	{

		enum PCICapabilityIDs
		{
			PCICapMSI = 0x5,
		};

		enum PCIConfigRegisters
		{
			PCIDeviceID = 0x2,
			PCIVendorID = 0x0,
			PCIStatus = 0x6,
			PCICommand = 0x4,
			PCIClassCode = 0xB,
			PCISubclass = 0xA,
			PCIProgIF = 0x9,
			PCIRevisionID = 0x8,
			PCIBIST = 0xF,
			PCIHeaderType = 0xE,
			PCILatencyTimer = 0xD,
			PCICacheLineSize = 0xC,
			PCIBAR0 = 0x10,
			PCIBAR1 = 0x14,
			PCIBAR2 = 0x18,
			PCIBAR3 = 0x1C,
			PCIBAR4 = 0x20,
			PCIBAR5 = 0x24,
			PCICardbusCISPointer = 0x28,
			PCISubsystemID = 0x2E,
			PCISubsystemVendorID = 0x2C,
			PCIExpansionROMBaseAddress = 0x30,
			PCICapabilitiesPointer = 0x34,
			PCIMaxLatency = 0x3F,
			PCIMinGrant = 0x3E,
			PCIInterruptPIN = 0x3D,
			PCIInterruptLine = 0x3C,
		};
		enum PCIVectors
		{
			PCIVectorLegacy = 0x1, // Legacy IRQ
			PCIVectorAPIC = 0x2,   // I/O APIC
			PCIVectorMSI = 0x4,	   // Message Signaled Interrupt
			PCIVectorAny = 0x7,	   // (PCIVectorLegacy | PCIVectorAPIC | PCIVectorMSI)
		};

		struct PCIMSICapability
		{
			union
			{
				struct
				{
					uint32_t capID : 8;		  // Should be PCICapMSI
					uint32_t nextCap : 8;	  // Next Capability
					uint32_t msiControl : 16; // MSI control register
				} __attribute__((packed));
				uint32_t register0;
			};
			union
			{
				uint32_t addressLow; // Interrupt Message Address Low
				uint32_t register1;
			};
			union
			{
				uint32_t data;		  // MSI data
				uint32_t addressHigh; // Interrupt Message Address High (64-bit only)
				uint32_t register2;
			};
			union
			{
				uint32_t data64; // MSI data when 64-bit capable
				uint32_t register3;
			};

			inline void SetData(uint32_t d)
			{
				if (msiControl & PCI_CAP_MSI_CONTROL_64)
				{
					data64 = d;
				}
				else
				{
					data = d;
				}
			}

			inline uint32_t GetData()
			{
				if (msiControl & PCI_CAP_MSI_CONTROL_64)
				{
					return data64;
				}
				else
				{
					return data;
				}
			}

			inline void SetAddress(int cpu)
			{
				addressLow = PCI_CAP_MSI_ADDRESS_BASE | (static_cast<uint32_t>(cpu) << 12);
				if (msiControl & PCI_CAP_MSI_CONTROL_64)
				{
					addressHigh = 0;
				}
			}
		} __attribute__((packed));

		enum PCIConfigurationAccessMode
		{
			Legacy,	  // PCI
			Enhanced, // PCI Express
		};

		class PCIDevice;

		class PeripheralComponentInterconnectController
		{
		public:
			//hardware::Port32Bit dataPort;
			//hardware::Port32Bit commandPort;

			PeripheralComponentInterconnectController(drivers::UsefulDevices *driverManager, myos::hardware::InterruptManager* interrupts);
			~PeripheralComponentInterconnectController();

			uint16_t
			ReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

			static uint32_t ConfigReadDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

			static uint16_t ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
			static void ConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data);

			static uint8_t ConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
			static void ConfigWriteByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t data);

			bool CheckDevice(uint8_t bus, uint8_t device, uint8_t func);
			bool FindDevice(uint16_t deviceID, uint16_t vendorID);
			bool FindGenericDevice(uint16_t classCode, uint16_t subclass);
			PCIDevice &GetPCIDevice(uint16_t deviceID, uint16_t vendorID);
			PCIDevice &GetGenericPCIDevice(uint8_t classCode, uint8_t subclass);

			bool DeviceHasFunctions(uint16_t bus, uint16_t device);
			drivers::PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function);
			uintptr_t GetBaseAddressRegister(uint16_t bus,uint16_t device,uint16_t function,uint16_t bar);
			//List<PCIDevice *> GetGenericPCIDevices(uint8_t classCode, uint8_t subclass);
		};
	
		class PCIDevice
		{
		public:
			uint16_t deviceID;
			uint16_t vendorID;

			uint8_t bus;
			uint8_t slot;
			uint8_t func;

			uint8_t classCode;
			uint8_t subclass;

			//		Vector<uint16_t> *capabilities; // List of capability IDs

			uint8_t msiPtr;
			PCIMSICapability msiCap;
			bool msiCapable = false;

			inline uintptr_t GetBaseAddressRegister(uint8_t idx)
			{

				uintptr_t bar = PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, PCIBAR0 + (idx * sizeof(uint32_t)));
				if (!(bar & 0x1) /* Not IO */ && bar & 0x4 /* 64-bit */ && idx < 5)
				{
					
					bar |= static_cast<uintptr_t>(PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, PCIBAR0 + ((bar + 1) * sizeof(uint32_t)))) << 32;
				}

				return (bar & 0x1) ? (bar & 0xFFFFFFFFFFFFFFFC) : (bar & 0xFFFFFFFFFFFFFFF0);
			}

			inline bool BarIsIOPort(uint8_t idx)
			{
				return PeripheralComponentInterconnectController::ConfigReadDword(bus, slot, func, PCIBAR0 + (idx * sizeof(uint32_t))) & 0x1;
			}

			inline uint8_t GetInterruptLine()
			{
				return PeripheralComponentInterconnectController::ConfigReadByte(bus, slot, func, PCIInterruptLine);
			}

			inline void SetInterruptLine(uint8_t irq)
			{
				PeripheralComponentInterconnectController::ConfigWriteByte(bus, slot, func, PCIInterruptLine, irq);
			}

			inline uint16_t GetCommand()
			{
				return PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, PCICommand);
			}

			inline void SetCommand(uint16_t val)
			{
				PeripheralComponentInterconnectController::ConfigWriteWord(bus, slot, func, PCICommand, val);
			}

			inline void EnableBusMastering()
			{
				PeripheralComponentInterconnectController::ConfigWriteWord(bus, slot, func, PCICommand, PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, PCICommand) | PCI_CMD_BUS_MASTER);
			}

			inline void EnableInterrupts()
			{
				PeripheralComponentInterconnectController::ConfigWriteWord(bus, slot, func, PCICommand, PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, PCICommand) & (~PCI_CMD_INTERRUPT_DISABLE));
			}

			inline void EnableMemorySpace()
			{
				PeripheralComponentInterconnectController::ConfigWriteWord(bus, slot, func, PCICommand, PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, PCICommand) | PCI_CMD_MEMORY_SPACE);
			}

			inline void EnableIOSpace()
			{
				PeripheralComponentInterconnectController::ConfigWriteWord(bus, slot, func, PCICommand, PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, PCICommand) | PCI_CMD_IO_SPACE);
			}

			inline void UpdateClass()
			{
				classCode = PeripheralComponentInterconnectController::ConfigReadByte(bus, slot, func, PCIClassCode);
				subclass = PeripheralComponentInterconnectController::ConfigReadByte(bus, slot, func, PCISubclass);
			}

			inline uint8_t HeaderType()
			{
				return PeripheralComponentInterconnectController::ConfigReadByte(bus, slot, func, PCIHeaderType);
			}

			inline uint8_t GetProgIF()
			{
				return PeripheralComponentInterconnectController::ConfigReadByte(bus, slot, func, PCIProgIF);
			}

			inline uint16_t Status()
			{
				return PeripheralComponentInterconnectController::ConfigReadWord(bus, slot, func, PCIStatus);
			}
			inline bool HasCapability(uint16_t capability)
			{
				return false;
			}

			
		};
	} // namespace PCI

} // namespace myos
