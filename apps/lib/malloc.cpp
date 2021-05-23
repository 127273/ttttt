#include <klikaos.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#define END_OF_INITIALISED_HEAP PAGE_SIZE

extern void* __user_app_end;

mblock_t *root_mblock;
uint64_t heap_start;
uint64_t heap_end;

size_t align_to(size_t addr, size_t align) {
  size_t align_mask = align - 1;
  if (addr & align_mask) {
    return (addr | align_mask) + 1;
  }
  else {
    return addr;
  }
}

mblock_t *find_heap_block(size_t size) {
  HEAP_WALKER(mb) {
    if (mb->free && mb->size > size) { // not >= ... little hack to not match last block in a byte 
      return mb;
    }
    if(mb->next == NULL)return NULL;
  }
  return NULL;
}

void debug_heap_dump() {
  DEBUG("Heap dump");
  HEAP_WALKER(mb) {
    DEBUG("Heap: mb: 0x%X mag:%X siz:%i free:%i", mb, mb->magic, mb->size, mb->free);
  }
}

void sbrk(size_t size) {
  DEBUG("Heap: Sbrk called : %i\n", size);

  // Ask more memory from kernel
  syscall(SYSCall_memory_sbrk);

  mblock_t *mb = root_mblock;
  while(mb->next != NULL) { mb = mb->next; }
  mb->size += PAGE_SIZE;
  heap_end += PAGE_SIZE;
  debug_heap_dump();
  DEBUG("Heap: Heap after srbk 0x%X\n", heap_end);
}


mblock_t *split_heap_block(mblock_t *mb, size_t size) {
  mb->free = false;
	// No splitting since new block can't fit
	if (mb->size < size + sizeof(mblock_t)) {
		return mb;
	}
  size_t old_size = mb->size;
  mblock_t *old_next = mb->next;
  uint8_t *ptr = (uint8_t*)mb;

  mb->size = size ;
  
  mblock_t *next_mb = (mblock_t*)(ptr + sizeof(mblock_t) + mb->size);
   
  mb->next = next_mb;
   
  // mmap is exist  but phy memory is not exist
  next_mb->magic = MBLOCK_MAGIC;

  next_mb->size = old_size - size - sizeof(mblock_t);
 
  next_mb->next = old_next;
  
  next_mb->free = true;


  return mb;
}

void *malloc(size_t size) {
  // Align next block to 64bits
  //return mallok(size);
  size = align_to(size, 8);
  mblock_t *Res = root_mblock;
  while(Res->next)
    Res = Res->next;
  //DEBUG("Total useful size:0x %x, Need size:0x %x\n", Res->size, size);
  mblock_t *mb = find_heap_block(size);
  if (mb == NULL) {
    sbrk(size);
    DEBUG("NULL\n");
    return malloc(size);
  } 
  else {
    DEBUG("split\n");
    mb = split_heap_block(mb, size);
  }
  return ((uint8_t*)mb) + sizeof(mblock_t);
}

void free(void *ptr) {
  mblock_t *mb = (mblock_t *)(((uint8_t*)ptr) - sizeof(mblock_t));
  if (mb->magic == MBLOCK_MAGIC) {
    mb->free = true;
  }
}

void *calloc(size_t num, size_t size) {
  void *ptr = malloc(num * size);
  memset(ptr, 0, num * size);
  return ptr;
}

extern "C" void init_heap() {
  heap_start = (uint64_t)&__user_app_end;
  heap_end = END_OF_INITIALISED_HEAP;
  root_mblock = (mblock_t*)heap_start;
  root_mblock->free = true;
  root_mblock->magic = MBLOCK_MAGIC;
  root_mblock->size = heap_end - heap_start - sizeof(mblock_t);
  root_mblock->next = NULL;
}



void *mallok(size_t size) {
	uint64_t k = 0;
	syscall(32, size, &k);
  return (void *)k;

}