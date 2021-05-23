#include "multiboot2.h"
#include <stdint.h>

namespace myos
{
    namespace grub
    {
        class GrubMessage{

            private:
                uint64_t TotalMem;
                uint64_t RAM; /* 内存 */
                uint64_t ROM; /* 储存 */

                struct multiboot_tag_string *tag_string;
                struct multiboot_tag_name   tag_name;
                struct multiboot_tag_module tag_module;
                struct multiboot_tag_basic_meminfo  tag_basic_meminfo;
                struct multiboot_tag_bootdev    tag_bootdev;
                struct multiboot_tag_mmap   tag_mmap;
                struct multiboot_tag_vbe    tag_vbe;
                struct multiboot_tag_framebuffer*    tag_framebuffer;
                struct multiboot_tag_elf_sections   tag_elf_sections;
                struct multiboot_tag_apm    tag_apm;
                struct multiboot_tag_efi32  tag_efi32;
                struct multiboot_tag_efi64  tag_efi64;
                struct multiboot_tag_smbios tag_smbios;
                struct multiboot_tag_old_acpi   tag_old_acpi;
                struct multiboot_tag_new_acpi   tag_new_acpi;
                struct multiboot_tag_network    tag_netwotk;
                struct multiboot_tag_efi_mmap   tag_efi_mmap;
                struct multiboot_tag_not_terminated tag_not_terminated;
                struct multiboot_tag_efi32_ih   tag_efi32_ih;
                struct multiboot_tag_efi64_ih   tag_efi64_ih;
                struct multiboot_tag_load_base_addr tag_load_base_addr;
            public:

                GrubMessage(multiboot_info_t* mbi_phys);
                uint32_t GetFrameBufferWidth();
                uint32_t GetFrameBufferHeight();
                multiboot_tag_framebuffer* GetFrameBuffer();

                
                uint64_t GetRAM();
                uint64_t GetROM();
                uint64_t GetTotalMeM();
        };
    }
}