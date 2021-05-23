#include "multiboot_head.h"
#include <vesa.h>
#include <kernel.h>
using namespace myos;
using namespace grub;

uint32_t GrubMessage::GetFrameBufferWidth()
{
    return tag_framebuffer->tag_framebuffer_common.framebuffer_width;
}

uint32_t  GrubMessage::GetFrameBufferHeight()
{
    return tag_framebuffer->tag_framebuffer_common.framebuffer_height;
}

multiboot_tag_framebuffer* GrubMessage::GetFrameBuffer()
{
    return tag_framebuffer;
}

uint64_t GrubMessage::GetRAM()
{
    return RAM;
}

uint64_t GrubMessage::GetROM()
{
    return ROM;
}

uint64_t GrubMessage::GetTotalMeM()
{
    return TotalMem;
}


GrubMessage::GrubMessage(multiboot_info_t* mbi)
{
    
    multiboot_tag_t *tag;
  for (
    tag = (multiboot_tag_t *) mbi->tags;
    tag->type != MULTIBOOT_TAG_TYPE_END;
    tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    )     
    {
        switch (tag->type)
        {
                
            case MULTIBOOT_TAG_TYPE_CMDLINE:
            {
               tag_string = (struct multiboot_tag_string*)tag;
                //tag_string.multiboot_tag = tmptag->multiboot_tag;
                //tag_string.string = tmptag->string;
                DEBUG   ("Command line = %s\n", 
                        tag_string->string);
                break;
            }

            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            {
                //struct multiboot_tag_name * tmptag = (struct multiboot_tag_name *) tag;
                //tag_name.multiboot_tag = tmptag->multiboot_tag;
                //tag_name.string = tmptag->string;
                DEBUG   ("Boot loader name = %s\n",
                        ((struct multiboot_tag_string *) tag)->string);
                break;
            }

            case MULTIBOOT_TAG_TYPE_MODULE:
            {
                //struct multiboot_tag_module* tmptag = (struct multiboot_tag_module*)tag;
                //tag_module.cmdline = tmptag->cmdline;
                //tag_module.mod_end = tmptag->mod_end;
                //tag_module.mod_start = tmptag->mod_start;
                //tag_module.multiboot_tag = tmptag->multiboot_tag;
                DEBUG   ("Module at 0x%x-0x%x. Command line %s\n",
                        ((struct multiboot_tag_module *) tag)->mod_start,
                        ((struct multiboot_tag_module *) tag)->mod_end,
                        ((struct multiboot_tag_module *) tag)->cmdline);
            break;
            }

            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            {
                //struct multiboot_tag_basic_meminfo *tmptag = (struct multiboot_tag_basic_meminfo *) tag;
                //tag_basic_meminfo.mem_lower  = tmptag->mem_lower;
                //tag_basic_meminfo.mem_upper = tmptag->mem_upper;
                //tag_basic_meminfo.multiboot_tag = tmptag->multiboot_tag;
                DEBUG   ("mem_lower = %uKB, mem_upper = %uKB\n",
                        ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
                        ((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
            }
            break;

            case MULTIBOOT_TAG_TYPE_BOOTDEV:
            {
               // struct multiboot_tag_bootdev *tmptag = (struct multiboot_tag_bootdev*)tag;
                //tag_bootdev.biosdev = tmptag->biosdev;
                //tag_bootdev.part = tmptag->part;
                //tag_bootdev.slice = tmptag->slice;
                //tag_bootdev.multiboot_tag = tmptag->multiboot_tag;
                DEBUG ("Boot device 0x%x,%u,%u\n",
                    ((struct multiboot_tag_bootdev *) tag)->biosdev,
                    ((struct multiboot_tag_bootdev *) tag)->slice,
                    ((struct multiboot_tag_bootdev *) tag)->part);
            break;
            }

            case MULTIBOOT_TAG_TYPE_MMAP:
            {
                multiboot_memory_map_t *mmap;

                for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
                    (multiboot_uint8_t *) mmap 
                    < (multiboot_uint8_t *) tag + tag->size;
                    mmap = (multiboot_memory_map_t *) 
                    ((unsigned long) mmap
                        + ((struct multiboot_tag_mmap *) tag)->entry_size))
                {
                    if(mmap->type == 1)
                        RAM += mmap->len;

                    TotalMem += mmap->len;
                    DEBUG (" base_addr = 0x%x%x,"
                        " length = 0x%x%x, type = 0x%x\n",
                        (unsigned) (mmap->addr >> 32),
                        (unsigned) (mmap->addr & 0xffffffff),
                        (unsigned) (mmap->len >> 32),
                        (unsigned) (mmap->len & 0xffffffff),
                        (unsigned) mmap->type);
                }
            }
            break;

            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
            {
                tag_framebuffer = (struct multiboot_tag_framebuffer *) tag;
               // struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
                //extract_framebuffer_info(tagfb);
               /* multiboot_uint32_t    color = 1;
                
                unsigned i;
                struct multiboot_tag_framebuffer *tagfb
                = (struct multiboot_tag_framebuffer *) tag;
                //void *fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;

                switch (tagfb->common.framebuffer_type)
                {
                case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
                    {
                    unsigned best_distance, distance;
                    struct multiboot_color *palette;
                
                    palette = tagfb->framebuffer_palette;

                    color = 0;
                    best_distance = 4*256*256;
                
                    for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++)
                        {
                        distance = (0xff - palette[i].blue) 
                            * (0xff - palette[i].blue)
                            + palette[i].red * palette[i].red
                            + palette[i].green * palette[i].green;
                        if (distance < best_distance)
                            {
                            color = i;
                            best_distance = distance;
                            }
                        }
                    }
                    break;

                case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                    color = ((1 << tagfb->framebuffer_blue_mask_size) - 1) 
                    << tagfb->framebuffer_blue_field_position;
                    break;

                case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                    color = '\\' | 0x0100;
                    break;

                default:
                    color = 0xffffffff;
                    break;
                }*/
            }
        }
    }
}
            