#include <fat.h>
#include <isr.h>
#include <kernel.h>
// uint32_t syscall_file_open(char *filename, mode, scratch_buffer, *fileinfo_out)
void syscall_file_open(isr_ctx_t *regs) {
	uint8_t *filename = (uint8_t*)regs->rdi;
	uint8_t mode = regs->rsi & 0xFF;
	uint8_t *scratch_buffer = (uint8_t *)regs->rdx;
	FILEINFO *file_info_out = (FILEINFO*)(regs->rcx);
	uint64_t * result = (uint64_t*)(regs->r8);
	*result = DFS_OpenFile(&dosfs_volume_info, filename, mode, scratch_buffer, file_info_out);
}

// syscall_file_read(*fileinfo, scratch_buffer, ptr, *read_count, size)
void syscall_file_read(isr_ctx_t *regs) {
	FILEINFO *file_info = (FILEINFO*)(regs->rdi);
	uint8_t *scratch_buffer = (uint8_t *)(regs->rsi);
	uint8_t *ptr = (uint8_t *)(regs->rdx);
	uint32_t *read_bytes = (uint32_t*)(regs->rcx);
	size_t size = regs->r8;

	DFS_ReadFile(file_info, scratch_buffer, ptr, read_bytes, size);
	
	
}
//
// syscall_file_write(*fileinfo, scratch_buffer, ptr, *written_count, size)
uint64_t syscall_file_write(isr_ctx_t *regs) {
	FILEINFO *file_info = (FILEINFO*)regs->rdi;
	uint8_t *scratch_buffer = (uint8_t *)regs->rsi;
	uint8_t *ptr = (uint8_t *)regs->rdx;
	uint32_t *written_bytes = (uint32_t*)regs->rcx;
	uint64_t size = regs->r8;

	return DFS_WriteFile(file_info, scratch_buffer, ptr, written_bytes, size);
}

void syscall_file_write2(isr_ctx_t *regs) {
	FILEINFO *file_info = (FILEINFO*)regs->rdi;
	uint8_t *scratch_buffer = (uint8_t *)regs->rsi;
	uint8_t *ptr = (uint8_t *)regs->rdx;
	uint32_t *written_bytes = (uint32_t*)regs->rcx;
	//uint64_t size = regs->r8;
	uint64_t *result = (uint64_t*)(regs->r8);
	*result = DFS_WriteFile(file_info, scratch_buffer, ptr, written_bytes, regs->r8);
}

// syscall_dir_open(char* dirname, *dirinfo_out)
void syscall_dir_open(isr_ctx_t *regs) {
	uint8_t *dirname = (uint8_t*)regs->rdi;
	DIRINFO *dir_info_out = (DIRINFO*)regs->rsi;
	uint64_t* result = (uint64_t*)(regs->rdx);
	*result = DFS_OpenDir(&dosfs_volume_info, dirname, dir_info_out);
}

// syscall_dir_read_next(*dirinfo, *dirent_out)
void syscall_dir_read_next(isr_ctx_t *regs) {
	DIRINFO *dir_info = (DIRINFO*)regs->rdi;
	DIRENT *dirent_out = (DIRENT*)regs->rsi;
	uint64_t* result = (uint64_t*)(regs->rdx);
	*result = DFS_GetNext(&dosfs_volume_info, dir_info, dirent_out);
}
