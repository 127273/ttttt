#pragma once
#include <stdint.h>
typedef struct window_struct_kernel{
	uint64_t handle;
	int x;
	int y;
	int z;
	int width;
	int height;
	uint32_t attributes;
	int message_queue_index;
	task_t* parent_task;
	message_t message_queue[MAX_MESSAGE_QUEUE_LENGTH];
	context_t context;
} window_t;

struct window_struct {
	// Basic params
	window_handle handle;
	int id;
	int type;
	int x, y;
	int width, height;
	int state;
	char* title;

	// Win proc
	window_procedure_t window_procedure;
	window_procedure_t default_procedure;

	// Extended parameters (based on type ... check TYPE_EXT macro e.g. WINDOW_EXT)
	void *ext;

	// Linked list (in fact it should be general tree to have proper Z order)
	struct window_struct *next;
	struct window_struct *parent;
};
