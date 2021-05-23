#include <mmu_paging.h>
#include <mmu_frames.h>
#include <mmu_heap.h>
#include <kernel.h>
#include <memory.h>
#include <stdint.h>
#include <string.h>
#include <vesa.h>
#include <x86.h>
#include <isr.h>
using namespace myos::Graphical;
pml4e_t pml4e[512] __attribute__((aligned(4096)));
pdpe_t  pdpe[512] __attribute__((aligned(4096)));
pde_t   pde[512] __attribute__((aligned(4096)));

pdpe_t  pdpe_user[512] __attribute__((aligned(4096)));
pde_t   pde_user[512] __attribute__((aligned(4096)));

pdpe_t  pdpe_krnluser[512] __attribute__((aligned(4096)));
pde_t  pde_krnluser[512] __attribute__((aligned(4096)));

pdpe_t  pdpe_video[512] __attribute__((aligned(4096)));
pde_t   pde_video[512] __attribute__((aligned(4096)));

static void init_paging_kernel() {
  // Setup new PAGING DIRECTORY
  memset(&pml4e, 0, sizeof(pml4e_t) * 512);
  memset(&pdpe, 0, sizeof(pdpe_t) * 512);
  //memset(&pde, 0, sizeof(pde_t) * 512);
  memset(&pdpe_krnluser, 0, sizeof(pdpe_t) * 512);
  memset(&pde_krnluser, 0, sizeof(pde_t) * 512);

  // Kernel space
  //pml4e[0].all = TO_PHYS_U64(&pdpe) | 3;
  pml4e[0x100].all = TO_PHYS_U64(&pdpe) | 3;    // KERNEL_VMA: Present + Write (0x100 KERNEL_START_MEMORY)
  pml4e[0x1C0].all = TO_PHYS_U64(&pdpe_krnluser) | 3; // KERNEL_USER TO KRNLVMA: Present + Write (0x1C0 KERNEL_USER_VIEW_START_MEMORY)
  pdpe[0].all = TO_PHYS_U64(&pde) | 3; 
  pdpe_krnluser[0].all = TO_PHYS_U64(&pde_krnluser) | 3; 

  uint64_t num_pages = TO_PHYS_U64(heap_end) / PAGE_SIZE;
  DEBUG("MMU[paging]: Krnl space pages needed: %i\n", num_pages);
  
  pdpe[0].all = (0 * PAGE_SIZE_1G) | PAGE_PRESENT_CPL0;
  pdpe[3].all = (3 * PAGE_SIZE_1G) | PAGE_PRESENT_CPL0;
  //pdpe[4].all = (4 * PAGE_SIZE_1G) | PAGE_PRESENT_CPL0;
  //kernel can mapping everywhere(assume memory is 8G)
  /* 中文  i<= num_pages  但是我不管 我需要弄4GB 以便内存访问 PCI设备 */
  // for (uint64_t i = 0; i < 4; i++)
  // {
  //   //
  //   pdpe[i].all = (i * PAGE_SIZE_1G) | PAGE_PRESENT_CPL0; // Presetn + Write + Large (1GB)
   
  // }
  // for (uint64_t i = 0; i < 4; i++)
  //   pde[i].all = (i * PAGE_SIZE) | PAGE_PRESENT_CPL0; // Presetn + Write + Large (2MB)
}

int map_2m_page()
{
  // for (uint64_t i = 0; i < 512; i++)
  // {
  //     if(!pde[i].all)
  //     {
  //       pde[i].all = (i * PAGE_SIZE) | PAGE_PRESENT_CPL0; // Presetn + Write + Large (2MB)
  //       return 0;
  //     }
  // }
  // return 1;
}

static void init_paging_user() {
  // User space
  memset(&pdpe_user, 0, sizeof(pdpe_t) * 512);
  memset(&pde_user, 0, sizeof(pde_t) * 512);
  // change 0x to 0xff
  pml4e[0].all = TO_PHYS_U64(&pdpe_user) | 0x7; // USER_VMA: Present + Write + CPL3
  pml4e[0x110].all = TO_PHYS_U64(&pdpe_user) | 0x7;
  pdpe_user[0].all = TO_PHYS_U64(&pde_user) | 0x07; 

  //user can mapping 1-7G (assume memory is 8G)
   for (uint64_t i = 1; i < 8; i++)
   {
  //   //
     pdpe_user[i].all = ((i - 1) * PAGE_SIZE_1G) | PAGE_PRESENT_CPL3; // Presetn + Write + Large (1GB)
   }
 //pdpe 0 is 2M memory total 1G
      for (uint64_t i = 0; i < 512; i++)
     {
  // // //   //
       pde_user[i].all = (i * PAGE_SIZE_2M) | PAGE_PRESENT_CPL3; // Presetn + Write + Large (1GB)
     }
}

static void init_paging_video() {
  uint64_t video_framebuffer = vesa_video_info.linear_addr;
  DEBUG("MMU: Video memory %0x\n", video_framebuffer);

  memset(&pdpe_video, 0, sizeof(pdpe_t) * 512);
  memset(&pde_video, 0, sizeof(pde_t) * 512);

  pml4e[0x180].all = TO_PHYS_U64(&pdpe_video) | 3; // KERNEL_VMA: Present + Write (0x180 : KERNEL_VIDEO_MEMORY)
  pdpe_video[0].all = TO_PHYS_U64(&pde_video) | 3; 
  // Dummy way : map whole PDE ... (more than needed but it's simpel enough)
  // TODO : if user have memory on this address - there will be a PROBLEM :D
  for (uint64_t i=0; i<256; i++) {
    pde_video[i].all = (video_framebuffer + (i * PAGE_SIZE)) | 0x83; // Present + Write + Large (2MB)
  }  
}

void init_kernel_paging(uint64_t RAM) {
  DEBUG("MMU[paging] PAGING: init ...");
  init_mmu_frames(RAM);
  init_paging_kernel();
  init_paging_user();
  init_paging_video();
  x86_set_cr3(TO_PHYS_U64(pml4e));
  init_mmu_heap();
}


template <class T = uintptr_t, class F>
inline const T get_mem_addr(F addr) { return reinterpret_cast<T>((uintptr_t)(addr) + 0xffff800000000000); }
template <class T = uintptr_t, class F>
inline const T get_rmem_addr(F addr) { return reinterpret_cast<T>((uintptr_t)(addr)-0xffff800000000000); }
template <class T = uintptr_t, class F>
inline const T get_kern_addr(F addr) { return reinterpret_cast<T>((uintptr_t)(addr) + 0xffffffff80000000); }
template <class T = uintptr_t, class F>
inline const T get_rkern_addr(F addr) { return reinterpret_cast<T>((uintptr_t)(addr)-0xffffffff80000000); }

typedef uint64_t main_page_table;
int map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags)
{
    uint64_t pml4_entry = PML4_GET_INDEX(virt_addr);
    uint64_t pdpt_entry = PDPT_GET_INDEX(virt_addr);
    uint64_t pd_entry = PAGE_DIR_GET_INDEX(virt_addr);
    uint64_t pt_entry = PAGE_TABLE_GET_INDEX(virt_addr);

    main_page_table *pdpt, *pd, *pt;

    if (pml4e[pml4_entry].all & BIT_PRESENT)
    {
        pdpt = get_mem_addr<uint64_t *>(pml4e[pml4_entry].all & FRAME_ADDR);
    }
    else
    {
        // pdpt = (uint64_t *)get_mem_addr(pmm_alloc_zero(1));
        // pml4e[pml4_entry].all = (uint64_t)(get_rmem_addr((uint64_t)pdpt)) | PAGE_TABLE_FLAGS;
        //get_current_cpu()->page_table[pml4_entry] = (uint64_t)(get_rmem_addr((uint64_t)pdpt)) | PAGE_TABLE_FLAGS;
    }

    if (pdpt[pdpt_entry] & BIT_PRESENT)
    {
        pd = get_mem_addr<uint64_t *>(pdpt[pdpt_entry] & FRAME_ADDR);
    }
    else
    {
        // pd = get_mem_addr<uint64_t *>(pmm_alloc_zero(1));
        // pdpt[pdpt_entry] = get_rmem_addr(pd) | PAGE_TABLE_FLAGS;
    }

    if (pd[pd_entry] & BIT_PRESENT)
    {
        pt = get_mem_addr<uint64_t *>(pd[pd_entry] & FRAME_ADDR);
    }
    else
    {
        // pt = get_mem_addr<uint64_t *>(pmm_alloc_zero(1));
        // pd[pd_entry] = get_rmem_addr(pt) | PAGE_TABLE_FLAGS;
    }

    pt[pt_entry] = (uint64_t)(phys_addr | flags);

    return 0;
}