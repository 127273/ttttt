#include <stdio.h>
#include <kernel.h>
#include <assert.h>
#include <mmu_heap.h>
#include <string.h>
#include <dosfs.h>
#include <fat.h>


FILE* fopen(char* filename, const char* mode) {
  assert(mode != NULL);

  FILE file;

  if (!strcmp(mode, "r")) {
    DEBUG("open \n\n");
    if (int i = DFS_OpenFile(&dosfs_volume_info, (uint8_t*)filename, DFS_READ, file.scratch_sector, &file.file_info)) {
      DEBUG("DOSFS: error opening file %s parameter: %d\n", filename, i);
      return NULL;
    }
  }
  else {
    DEBUG("DOSFS: Unsupported mode. only \"r\" supported!\n");
    return NULL;
  }

  // TODO: Can this be more cleaner?? 
  FILE *ret_file = (FILE*)malloc(sizeof(FILE));
  *ret_file = file;
  return ret_file;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  uint32_t read_count;

  DFS_ReadFile(&stream->file_info, stream->scratch_sector, (uint8_t*)ptr, &read_count, size * nmemb);

  return read_count;
}

int fclose(FILE *stream) {
  free(stream);
  return 0;
}

uint32_t fsize(FILE *stream) {
  return stream->file_info.filelen;
}