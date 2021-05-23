#include <window_manager.h>
#include <mmu_heap.h>
#include <process.h>
#include <kernel.h>
#include <string.h>
#include <vesa.h>
#include <gfx.h>
#include <isr.h>

// syscall_windows_create(int x, int y, int width, int height, char* title)
void syscall_windows_create(isr_ctx_t *regs) {
	// window_t* temp = (window_t*)(regs->rdi);
	// window_t* win = window_create(temp->x, temp->y, temp->width, temp->height, temp->attributes);
	// temp->handle = 1000 ;//win->handle;
	
	// DEBUG("k %d \n", temp->handle);
	uint64_t* t = (uint64_t*)( regs->rdi);
	int x = *t;
	int y = regs->rsi;
	int width = regs->rdx;
	int height = regs->rcx;
	uint32_t attributes = (uint32_t) regs->r8;
	
	window_t* win = window_create(x, y, width, height, attributes);
	*t = win->handle;

	//return win->handle;
}

// syscall_windows_present(uint32_t handle, context_t*)
uint64_t syscall_windows_present(isr_ctx_t *regs) {
	window_t* win = window_find(regs->rdi);
	if (win != NULL) {
		DEBUG("syscall 5 is not null\n");
		context_t* context = (context_t*)(regs->rsi);
		window_present_context(win, context);
		return 1;
	}
	DEBUG("syscall 5 is  null\n");
	return 0;
}

void syscall_get_VGA_information(isr_ctx_t *regs)
{
	//DEBUG("width %x %x %d\n", vesa_video_info.width, &regs->rdi, regs->rdi);
	//uint16_t *mem = (uint16_t*)(regs->rdi) ;
	//mem[0] = vesa_video_info.width;

	video_info_struct* temp_vesa = (video_info_struct*)(regs->rdi);
	//does not give the address maybe is dangerous  
	//temp_vesa->addr = vesa_video_info.addr;
	//temp_vesa->linear_addr = vesa_video_info.linear_addr;
	temp_vesa->bits = vesa_video_info.bits;
	temp_vesa->height = vesa_video_info.height;
	temp_vesa->pitch = vesa_video_info.pitch;
	temp_vesa->type = vesa_video_info.type;
	temp_vesa->width = vesa_video_info.width;
}