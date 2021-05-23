#include <mmu_heap.h>

#include <mmu_frames.h>
#include <mmu_paging.h>
#include <kernel.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <x86.h>
#include <isr.h>

mblock_t *root_mblock;

mblock_t *find_heap_block(uintptr_t size) {
  // DEBUG("MMU[heap]: Heap find_heap_block %i\n", size);
  HEAP_WALKER(mb) {
    //mb.free is a problem
   //DEBUG("size3: %x %d \t", mb->size, mb->free);
    if ((mb->free) && (mb->size > size)) { // not >= ... little hack to not match last block in a byte 
     // DEBUG("size: %x %d \t", mb->size, mb->free);
      return mb;
    }
   // if(mb->next == NULL) return NULL;
  }
  return NULL;
}

void debug_heap_dump() {
  // DEBUG("MMU[heap]: ------- HEAP DUMP --------\n");
  HEAP_WALKER(mb) {
    // DEBUG("MMU[heap]: mb: 0x%X mag:%X siz:%i free:%i\n", mb, mb->magic, mb->size, mb->free);
  }
  // DEBUG("MMU[heap]: --------------------------\n");
}


void sbrk(size_t size __UNUSED__) {
  // DEBUG("MMU[heap]: Sbrk called : %i\n", size);
  // Find last entry and m ap to it (we are never freeing pages)
  for (uint64_t i=0; i<4; i++) {//
    if (pdpe[i].all == 0) {
      uint64_t frame_address = (uint64_t)alloc_frame();
      pdpe[i].all = frame_address | PAGE_PRESENT_CPL0;
      // DEBUG("MMU[heap]: Sbrk new frame : %i 0x%X\n", i, frame_address);
      break;
    }
  }
  // x86_set_cr3(TO_PHYS_U64(pml4e));
  x86_tlb_flush_all();

  //Extend last memory block

  mblock_t *mb = root_mblock;
  while(mb->next != NULL) { mb = mb->next; }
  mb->size += PAGE_SIZE;
  heap_end += PAGE_SIZE;
  debug_heap_dump();
  // DEBUG("MMU[heap]: Heap after srbk 0x%X\n", TO_PHYS_U64(heap_end));
}

mblock_t *split_heap_block(mblock_t *mb, uintptr_t size) {
  mb->free = false;
	// No splitting since new block can't fit
	if (mb->size < (size + sizeof(mblock_t))) {
		return mb;
	}
  uintptr_t old_size = mb->size;
  mblock_t *old_next = mb->next;
  uint8_t *ptr = (uint8_t*)mb;

  mb->size = size;

  mblock_t *next_mb = (mblock_t*)(ptr + sizeof(mblock_t) + mb->size);
  mb->next = next_mb;
  next_mb->magic = MBLOCK_MAGIC;
  next_mb->size = old_size - size - sizeof(mblock_t);
  /** 可能是最后一个 也可能是中间的 */
  next_mb->next = old_next;
  next_mb->free = true;

  //DEBUG("ext_mb->size : %x", next_mb->size);
  // DEBUG("MMU[heap]: split prev: 0x%X mag:%X siz:%i free:%i\n", mb, mb->magic, mb->size, mb->free);
  // DEBUG("MMU[heap]: split next: 0x%X mag:%X siz:%i free:%i\n", next_mb, next_mb->magic, next_mb->size, next_mb->free);
//HEAP_WALKER(mb2)
//DEBUG("size: %x %d \t", mb2->size, mb2->free);
  return mb;
}

size_t align_to(size_t addr, size_t align) {
  size_t align_mask = align - 1;
  if (addr & align_mask) {
    return (addr | align_mask) + 1;
  }
  else {
    return addr;
  }
}

void *malloc(size_t size) {
  // Align size to 64bit
  // size & 0b111
  size_t old_size = size;
  size = align_to(size, 8);

  DEBUG("MMU[heap]: malloc(%i) alligned malloc(%i)\n", old_size, size);
  mblock_t *mb = find_heap_block(size);
  if (mb == NULL) {
    DEBUG("MMU[heap]: No more empty blocks ... sbrk-ing ...\n");
    sbrk(size);
    return malloc(size);
  } 
  else {
    // DEBUG("MMU[heap]: split block  ...\n");
    mb = split_heap_block(mb, size);
  }
  return ((uint8_t*)mb) + sizeof(mblock_t);
}

void *calloc(size_t num, size_t size) {
  void *ptr = malloc(num * size);
  memset(ptr, 0, num * size);
  return ptr;
}


void free(void *ptr) {
  mblock_t *mb = (mblock_t *)(((uint8_t*)ptr) - sizeof(mblock_t));
  if (mb->magic != MBLOCK_MAGIC) {
    HALT_AND_CATCH_FIRE("HEAP: free(0x%X) - freeing bad block.\n");
  }
  mb->free = true;
}

void init_mmu_heap() {
  root_mblock = (mblock_t*)heap_start;
  root_mblock->free = true;
  root_mblock->magic = MBLOCK_MAGIC;
  root_mblock->size = heap_end - heap_start - sizeof(mblock_t);
  root_mblock->next = NULL;
  // DEBUG("MMU[heap]: Heap init : start:0x%X end:0x%X\n", heap_start, heap_end);
   DEBUG("MMU[heap]: Heap first block size : %i\n", root_mblock->size);
   
}

#define PREFIX(func) k##func
extern void *PREFIX(malloc)(size_t);

void *operator new(size_t size)
{
  return malloc(size);
}

void *operator new[](size_t size)
{
  return malloc(size);
}

void *operator new(size_t size, void *ptr)
{
  return ptr;
}

void *operator new[](size_t size, void *ptr)
{
  return ptr;
}

void operator delete(void *ptr)
{
  free(ptr);
}

void operator delete[](void *ptr)
{
  free(ptr);
}
