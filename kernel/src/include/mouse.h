#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdint.h>

#define PACKETS_IN_PIPE 1024
#define DISCARD_POINT 32

#define MOUSE_IRQ 12

#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_ABIT   0x02
#define MOUSE_BBIT   0x01
#define MOUSE_WRITE  0xD4
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08

#define MOUSE_DEFAULT 0
#define MOUSE_SCROLLWHEEL 1
#define MOUSE_BUTTONS 2

#define MOUSE_LEFT_CLICK   		0x01
#define MOUSE_RIGHT_CLICK  		0x02
#define MOUSE_MIDDLE_CLICK 		0x04
#define MOUSE_MOUSE_SCROLL_UP 0x10
#define MOUSE_MOUSE_SCROLL_DOWN 0x20


void init_kernel_mouse(void);



#include <types.h>
#include <driver.h>
#include <port.h>
#include <isr.h>

namespace myos
{
    namespace drivers
    {
    
        
        
        class MouseDriver : public myos::hardware::InterruptHandler, public Driver
        {
            myos::hardware::Port8Bit dataport;
            myos::hardware::Port8Bit commandport;
            uint8_t interruptLine;
        public:
            static uint8_t mouse_cycle;
            static uint8_t mouse_byte[4];
            static int32_t mouse_x;
            static int32_t mouse_y;
            static int32_t mouse_x_difference;
            static int32_t mouse_y_difference;
            static int32_t mouse_buttons;

        public:
            MouseDriver(myos::hardware::InterruptManager* manager, uint8_t interruptLine);
            ~MouseDriver();
            virtual void HandleInterrupt(isr_ctx_t *regs);
            virtual void Activate();
        };

    }
}
    
#endif