#include <window_manager.h>
#include <messaging.h>
#include <process.h>
#include <kernel.h>
#include <string.h>
#include <isr.h>
#include <x86.h>

#define __UNUSED__ __attribute__((unused))

// syscall_messaging_get(message_t* msg_out, uint32_t handle)
void syscall_messaging_get(isr_ctx_t *regs) {
	message_t* msg = (message_t*)(regs->rdi);
	window_t* win = window_find(regs->rsi);
	
	DEBUG("win->message_queue_inde: %d\n",win->message_queue_index);
	if (win != NULL) {
		
		// If no events - go to WAIT state
		if(!window_pop_message(win, msg)) {
			task_list_current->state = PROCESS_STATE_WAIT;
			DEBUG("schedule \n");
			schedule();
		}
	}
	
}

void syscall_messaging_peek(isr_ctx_t *regs) {
	message_t* msg = (message_t*)regs->rdi;
	window_t* win = window_find(regs->rsi);
	uint64_t *t = &(regs->rdx);
	if (win != NULL) {
		*t =  window_pop_message(win, msg);
		//return window_pop_message(win, msg);
	}
	*t = true;
}

// syscall_messaging_create(message_t* msg, uint32_t handle)
void syscall_messaging_create(isr_ctx_t *regs) {
	message_t* msg = (message_t*)(regs->rdi);
	window_t* win = window_find(regs->rsi);
	if (win != NULL) {
		window_add_message(win, msg);
	}	
}
