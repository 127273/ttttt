#ifndef __VESA_H
#define __VESA_H

#include <multiboot2.h>
//#include <kernel.h>
//#include <multiboot.h>
#define VESA_FRAMEBUFFER_8 ((uint8_t*)KERNEL_VIDEO_MEMORY)
#define VESA_FRAMEBUFFER_32 ((uint32_t*)KERNEL_VIDEO_MEMORY)
#define VESA_FRAMEBUFFER_64 ((uint64_t*)KERNEL_VIDEO_MEMORY)

#define VIDEO_INFO_MEM_SIZE(vi) (vi.width * vi.height * (vi.bits/8))

typedef struct video_info_struct {
  uint64_t linear_addr;
  uint64_t addr;
  int width;
  int height;
  int bits;
  int pitch;
  uint8_t  type;
} video_info_t;

extern struct video_info_struct vesa_video_info;


void extract_framebuffer_info(struct multiboot_tag_framebuffer * tagfb);

namespace myos
{
  namespace Graphical
  {
      class Vesa
      {
        public:      
          video_info_struct* vesa_video;
          Vesa(struct multiboot_tag_framebuffer * tagfb);
     
      };
  }
}

#endif
