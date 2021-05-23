#ifndef MULTIBOOT_HEADER
#define MULTIBOOT_HEADER 1

/*  How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH                        32768
#define MULTIBOOT_HEADER_ALIGN                  8

/*  The magic field should contain this. */
#define MULTIBOOT2_HEADER_MAGIC                 0xe85250d6

/*  This should be in %eax. */
#define MULTIBOOT2_BOOTLOADER_MAGIC             0x36d76289

/*  Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN                     0x00001000

/*  Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN                    0x00000008

/*  Flags set in the ’flags’ member of the multiboot header. */

#define MULTIBOOT_TAG_ALIGN                  8
#define MULTIBOOT_TAG_TYPE_END               0
#define MULTIBOOT_TAG_TYPE_CMDLINE           1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MULTIBOOT_TAG_TYPE_MODULE            3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MULTIBOOT_TAG_TYPE_BOOTDEV           5
#define MULTIBOOT_TAG_TYPE_MMAP              6
#define MULTIBOOT_TAG_TYPE_VBE               7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER       8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS      9
#define MULTIBOOT_TAG_TYPE_APM               10
#define MULTIBOOT_TAG_TYPE_EFI32             11
#define MULTIBOOT_TAG_TYPE_EFI64             12
#define MULTIBOOT_TAG_TYPE_SMBIOS            13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD          14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW          15
#define MULTIBOOT_TAG_TYPE_NETWORK           16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP          17
#define MULTIBOOT_TAG_TYPE_EFI_BS            18
#define MULTIBOOT_TAG_TYPE_EFI32_IH          19
#define MULTIBOOT_TAG_TYPE_EFI64_IH          20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR    21

/* multiboot header */
#define MULTIBOOT_HEADER_TAG_END  0
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST  1
#define MULTIBOOT_HEADER_TAG_ADDRESS  2
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS  3
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS  4
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER  5
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN  6
#define MULTIBOOT_HEADER_TAG_EFI_BS        7
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI32  8
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64  9
#define MULTIBOOT_HEADER_TAG_RELOCATABLE  10

#define MULTIBOOT_ARCHITECTURE_I386  62
#define MULTIBOOT_ARCHITECTURE_MIPS32  4
#define MULTIBOOT_HEADER_TAG_OPTIONAL 1

#define MULTIBOOT_LOAD_PREFERENCE_NONE 0
#define MULTIBOOT_LOAD_PREFERENCE_LOW 1
#define MULTIBOOT_LOAD_PREFERENCE_HIGH 2

#define MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED 1
#define MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2
 
#ifndef ASM_FILE

typedef unsigned char           multiboot_uint8_t;
typedef unsigned short          multiboot_uint16_t;
typedef unsigned int            multiboot_uint32_t;
typedef unsigned long long      multiboot_uint64_t;

struct multiboot_header
{
  /*  Must be MULTIBOOT_MAGIC - see above. */
  multiboot_uint32_t magic;

  /*  ISA */
  multiboot_uint32_t architecture;

  /*  Total header length. */
  multiboot_uint32_t header_length;

  /*  The above fields plus this one must equal 0 mod 2^32. */
  multiboot_uint32_t checksum;
} __attribute__((packed));


struct multiboot_header_tag_base
{
  multiboot_uint16_t type;
  multiboot_uint16_t flags;
  multiboot_uint32_t size;
} __attribute__((packed));

/* Multiboot2 information request  type = 1*/
struct multiboot_header_tag_information_request
{
  struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t requests[0];
} __attribute__((packed));

/* The address tag of Multiboot2 header   type = 2 */
struct multiboot_header_tag_address
{
  struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t header_addr;
  multiboot_uint32_t load_addr;
  multiboot_uint32_t load_end_addr;
  multiboot_uint32_t bss_end_addr;
} __attribute__((packed));

/*  The entry address tag of Multiboot2 header   type = 3 */
struct multiboot_header_tag_entry_address
{
  struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t entry_addr;
} __attribute__((packed));

/* Flags tag  type = 4 */
struct multiboot_header_tag_console_flags
{
  struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t console_flags;
} __attribute__((packed));

/*  The framebuffer tag of Multiboot2 header   type = 5*/
struct multiboot_header_tag_framebuffer
{
   struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t width;
  multiboot_uint32_t height;
  multiboot_uint32_t depth;
};

/* Module alignment tag   type = 6 */
struct multiboot_header_tag_module_align
{
  struct multiboot_header_tag_base multiboot_header_tag;
} __attribute__((packed));

/* EFI boot services tag type = 7 */
struct multiboot_header_tag_boot
{
  struct multiboot_header_tag_base multiboot_header_tag;
} __attribute__((packed));

/* EFI i386 entry address tag of Multiboot2 header  type = 8*/
struct multiboot_header_tag_ef32
{
  struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t entry_addr;
} __attribute__((packed));

/* EFI amd64 entry address tag of Multiboot2 header  type = 9 */
struct multiboot_header_tag_ef64
{
  struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t entry_addr;
} __attribute__((packed));

/*  Relocatable header tag   type = 10 */
struct multiboot_header_tag_relocatable
{
  struct multiboot_header_tag_base multiboot_header_tag;
  multiboot_uint32_t min_addr;
  multiboot_uint32_t max_addr;
  multiboot_uint32_t align;
  multiboot_uint32_t preference;
} __attribute__((packed));

struct multiboot_color
{
  multiboot_uint8_t red;
  multiboot_uint8_t green;
  multiboot_uint8_t blue;
} __attribute__((packed));


struct multiboot_mmap_entry
{
  multiboot_uint64_t addr;
  multiboot_uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  multiboot_uint32_t type;
  multiboot_uint32_t zero;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;



/* BIOS informations  */

typedef struct multiboot_tag_base
{
  multiboot_uint32_t type;
  multiboot_uint32_t size;
} __attribute__((packed)) multiboot_tag_t;

typedef struct multiboot_info
{
    multiboot_uint32_t size;
    multiboot_uint32_t reserved;
    multiboot_tag_base tags[];
    // end tags
} __attribute__((packed)) multiboot_info_t;


/* Boot command line  type = 1 */
typedef struct multiboot_tag_string
{
  struct multiboot_tag_base  multiboot_tag;
  char string[0];
} __attribute__((packed))  multiboot_tag_string_t;

/* Boot loader name   type = 2 */
struct multiboot_tag_name
{
  struct multiboot_tag_base  multiboot_tag;
  char string[0];
} __attribute__((packed));

/* Modules  type = 3 */
struct multiboot_tag_module
{
 struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t mod_start;
  multiboot_uint32_t mod_end;
  char *cmdline;
} __attribute__((packed));

/* Basic memory information  type = 4 */
struct multiboot_tag_basic_meminfo
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t mem_lower;
  multiboot_uint32_t mem_upper;
} __attribute__((packed));

/*  BIOS Boot device  type = 5 */
struct multiboot_tag_bootdev
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t biosdev;
  multiboot_uint32_t slice;
  multiboot_uint32_t part;
} __attribute__((packed));

/*  Memory map  type = 6 */
struct multiboot_tag_mmap
{
 struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t entry_size;
  multiboot_uint32_t entry_version;
  struct multiboot_mmap_entry entries[0];  
} __attribute__((packed));

struct multiboot_vbe_info_block
{
  multiboot_uint8_t external_specification[512];
} __attribute__((packed));

struct multiboot_vbe_mode_info_block
{
  multiboot_uint8_t external_specification[256];
} __attribute__((packed));

/* VBE info  type = 7 */
struct multiboot_tag_vbe
{
 struct multiboot_tag_base  multiboot_tag;

  multiboot_uint16_t vbe_mode;
  multiboot_uint16_t vbe_interface_seg;
  multiboot_uint16_t vbe_interface_off;
  multiboot_uint16_t vbe_interface_len;

  struct multiboot_vbe_info_block vbe_control_info;
  struct multiboot_vbe_mode_info_block vbe_mode_info;
} __attribute__((packed));

/* Framebuffer info  type = 8*/
struct multiboot_tag_framebuffer_common
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint64_t framebuffer_addr;
  multiboot_uint32_t framebuffer_pitch;
  multiboot_uint32_t framebuffer_width;
  multiboot_uint32_t framebuffer_height;
  multiboot_uint8_t framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT     2
  multiboot_uint8_t framebuffer_type;
  multiboot_uint16_t reserved;
} __attribute__((packed));

struct multiboot_tag_framebuffer
{
  struct multiboot_tag_framebuffer_common tag_framebuffer_common;

  union
  {
    struct
    {
      multiboot_uint16_t framebuffer_palette_num_colors;
      struct multiboot_color framebuffer_palette[0];
    } __attribute__((packed));
    struct
    {
      multiboot_uint8_t framebuffer_red_field_position;
      multiboot_uint8_t framebuffer_red_mask_size;
      multiboot_uint8_t framebuffer_green_field_position;
      multiboot_uint8_t framebuffer_green_mask_size;
      multiboot_uint8_t framebuffer_blue_field_position;
      multiboot_uint8_t framebuffer_blue_mask_size;
    } __attribute__((packed));
  };
} __attribute__((packed));

/* ELF-Symbols  type = 9 */
struct multiboot_tag_elf_sections
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t num;
  multiboot_uint32_t entsize;
  multiboot_uint32_t shndx;
  char sections[0];
} __attribute__((packed));

/* APM table   type = 10 */
struct multiboot_tag_apm
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint16_t version;
  multiboot_uint16_t cseg;
  multiboot_uint32_t offset;
  multiboot_uint16_t cseg_16;
  multiboot_uint16_t dseg;
  multiboot_uint16_t flags;
  multiboot_uint16_t cseg_len;
  multiboot_uint16_t cseg_16_len;
  multiboot_uint16_t dseg_len;
} __attribute__((packed));

/* EFI 32-bit system table pointer type = 11*/
struct multiboot_tag_efi32
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t pointer;
} __attribute__((packed));

/* EFI 64-bit system table pointer type = 12*/
struct multiboot_tag_efi64
{
 struct multiboot_tag_base  multiboot_tag;
  multiboot_uint64_t pointer;
} __attribute__((packed));

/*  SMBIOS tables  type = 13 */
struct multiboot_tag_smbios
{
 struct multiboot_tag_base  multiboot_tag;
  multiboot_uint8_t major;
  multiboot_uint8_t minor;
  multiboot_uint8_t reserved[6];
  multiboot_uint8_t tables[0];
} __attribute__((packed));

/* ACPI old RSDP  type = 14 */
struct multiboot_tag_old_acpi
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint8_t rsdp[0];
} __attribute__((packed));

/* ACPI new RSDP  type = 15 */
struct multiboot_tag_new_acpi
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint8_t rsdp[0];
} __attribute__((packed));

/*Networking information  type = 16*/
struct multiboot_tag_network
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint8_t dhcpack[0];
} __attribute__((packed));

/* EFI memory map  type =17 */
struct multiboot_tag_efi_mmap
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t descr_size;
  multiboot_uint32_t descr_vers;
  multiboot_uint8_t efi_mmap[0];
} __attribute__((packed)); 

/*EFI boot services not terminated type = 18*/
struct multiboot_tag_not_terminated{
  struct multiboot_tag_base  multiboot_tag;
} __attribute__((packed));
/* EFI 32-bit image handle pointer type = 19 */
struct multiboot_tag_efi32_ih
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t pointer;
} __attribute__((packed));

/* EFI 32-bit image handle pointer type = 20 */
struct multiboot_tag_efi64_ih
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint64_t pointer;
} __attribute__((packed));

/* Image load base physical address   type = 21 */
struct multiboot_tag_load_base_addr
{
  struct multiboot_tag_base  multiboot_tag;
  multiboot_uint32_t load_base_addr;
} __attribute__((packed));

#endif /*  ! ASM_FILE */

#endif /*  ! MULTIBOOT_HEADER */
