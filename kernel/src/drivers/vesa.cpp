#include <kernel.h>
#include <stdbool.h>
#include <string.h>
#include <vesa.h>
#include <x86.h>
#include <kernel.h>

struct video_info_struct vesa_video_info;

using namespace myos;
using namespace Graphical;


Vesa::Vesa(struct multiboot_tag_framebuffer * tagfb)
{
  vesa_video = &vesa_video_info;
  //extract_framebuffer_info(tagfb);
  void *fb = (void *) (unsigned long) tagfb->tag_framebuffer_common.framebuffer_addr;       
  vesa_video_info.linear_addr = (uint64_t)fb;
  vesa_video_info.addr = (uint64_t)KERNEL_VIDEO_MEMORY;
  vesa_video_info.width = tagfb->tag_framebuffer_common.framebuffer_width;
  vesa_video_info.height = tagfb->tag_framebuffer_common.framebuffer_height;
  vesa_video_info.bits = tagfb->tag_framebuffer_common.framebuffer_bpp;
  vesa_video_info.pitch = tagfb->tag_framebuffer_common.framebuffer_pitch;
  vesa_video_info.type = tagfb->tag_framebuffer_common.framebuffer_type;
  DEBUG("VESA: Frame buffer addr: 0x%X\n", vesa_video_info.addr);
  DEBUG("VESA: Frame buffer linear addr: 0x%X\n", vesa_video_info.linear_addr);
  DEBUG("VESA: Frame buffer info: %i x %i : %ibpp\n", vesa_video_info.width, vesa_video_info.height, vesa_video_info.bits);
  DEBUG("VESA: Frame buffer pitch: %i\n", vesa_video_info.pitch);
  DEBUG("VESA: Frame buffer type: %i\n", vesa_video_info.type);
  //DEBUG("cbcvuscbbc %d\n", (vesa_video->width));
}
