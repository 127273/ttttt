#include <klikaos.h>
#include <windows.h>
#include <gfx.h>
#include <bmp.h>
#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>
#include <vesa.h>
#include "apps.h"
#include <syscalls.h>

message_t msg;
window_t  *window;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)

#define MSG_USER_BTN_APP (WINDOW_USER_MESSAGE + 10)

#define COLUMN_COUNT 12

//extern struct video_info_struct vesa_video_info;

bmp_image_t default_app_icon;
bmp_image_t wallpaper;

void start_app(char *filename) {
	syscall(SYSCall_process_from_file, filename);
}

video_info_t veas ;  
extern "C" int main() {


	
	syscall(SYSCall_get_VGA_information, &veas);
	
	DEBUG("width: %d %d\n", veas.width, veas.height);
	if((veas.height == 0) || (veas.width == 0))
	{
		DEBUG("desktop main fail veas.height, or veas.width = 0 \n");
		return 1;
	}
	// not more 8 filename 
	bmp_from_file("/apps/desktop/appicon.bmp", &default_app_icon);
	bmp_from_file("/assets/24wallp.bmp", &wallpaper);
	

	window = window_create(0, 0, veas.width, veas.height, "Applications", MSG_USER_WIN, WINDOW_ATTR_BOTTOM | WINDOW_ATTR_NO_DRAG, WINDOW_FRAME_NONE);
	bmp_blit(WINDOW_EXT(window)->context, &wallpaper, 0, 0);
	window_present(window);	

	populate_app_info();
	DEBUG("desktop create33 \n");
	int app_index, i;
	for (i = 0; i < app_count; i++) {
		if (apps[i]->visible) {
			DEBUG("app visable \n");
			button_t *btn = button_create(window, 20 + (app_index % COLUMN_COUNT) * 84, 10 + (app_index / COLUMN_COUNT) * 89, 64, 64 + 9, apps[i]->display_name, MSG_USER_BTN_APP + i);
			if (!strcmp("/apps/desktop/appicon.bmp", apps[i]->icon_path)) {
				button_set_image(btn, BUTTON_STATE_NORMAL, &default_app_icon);
			} else {
				bmp_image_t* app_icon = (bmp_image_t*)malloc(sizeof(bmp_image_t));
				DEBUG("filename %s\n", apps[i]->icon_path);
				bmp_from_file(apps[i]->icon_path, app_icon);
				button_set_image(btn, BUTTON_STATE_NORMAL, app_icon);
			}
			app_index++;
		}
	}

	DEBUG("desktop create\n");
	while(window_get_message(window, &msg)) {
		
		int app_index = msg.message - MSG_USER_BTN_APP;
		if (app_index >= 0 && app_index < app_count) {
			DEBUG("ss %s", apps[app_index]->executable_path);
			start_app(apps[app_index]->executable_path);
		}
		window_dispatch(window, &msg);
		DEBUG("desktop\n");
	}
	DEBUG("desktop oversss\n");
	return 0;
}
