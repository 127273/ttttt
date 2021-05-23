//#include <bits/stdint-uintn.h>
#include <process.h>
#include <window_manager.h>
#include <keyboard.h>
#include <isr.h>
#include <x86.h>
#include <pic.h>
#include <kernel.h>
#include <apic.h>
#define SCANCODE_MAX 57

message_t old_msg;

int keymap[][2] = {
	/* 0 */		{0, 0},      {ESC, 0},
	/* 2 */		{'1', '!'},  {'2', '@'},
	/* 4 */		{'3', '#'},  {'4', '$'},
	/* 6 */		{'5', '%'},  {'6', '^'},
	/* 8 */		{'7', '&'},  {'8', '*'},
	/* 10 */	{'9', '('},  {'0', ')'},
	/* 12 */	{'-', '_'},  {'=', '+'},
	/* 14 */	{'\b', '\b'},{'\t', 0},
	/* 16 */	{'q', 'Q'},  {'w', 'W'},
	/* 18 */	{'e', 'E'},  {'r', 'R'},
	/* 20 */	{'t', 'T'},  {'y', 'Y'},
	/* 22 */	{'u', 'U'},  {'i', 'I'},
	/* 24 */	{'o', 'O'},  {'p', 'P'},
	/* 26 */	{'[', '{'},  {']', '}'},
	/* 28 */	{'\n', 0},   {'/', '?'},
	/* 30 */	{'a', 'A'},  {'s', 'S'},
	/* 32 */	{'d', 'D'},  {'f', 'F'},
	/* 34 */	{'g', 'G'},  {'h', 'H'},
	/* 36 */	{'j', 'J'},  {'k', 'K'},
	/* 38 */	{'l', 'L'},  {';', ':'}, 
	/* 40 */	{';', ':'},  {'\'', '"'}, 
	/* 42 */	{LSHIFT, 0}, {']', '}'},
	/* 44 */	{'z', 'Z'},  {'x', 'X'},
	/* 46 */	{'c', 'C'},  {'v', 'V'},
	/* 48 */	{'b', 'B'},  {'n', 'N'},
	/* 50 */	{'m', 'M'},  {',', '<'},
	/* 52 */	{'.', '>'},  {';', ':'},
	/* 54 */	{RSHIFT, 0}, {0, 0},
	/* 56 */	{ALT, 0},    {' ', ' '},
	/* 58 */	{0, 0},      {KEY_F1, KEY_F1},
	/* 60 */	{KEY_F2, KEY_F2},   {KEY_F3, KEY_F3},
	/* 62 */	{KEY_F4, KEY_F4},   {KEY_F5, KEY_F5},
	/* 64 */	{KEY_F6, KEY_F6},   {KEY_F7, KEY_F7},
	/* 66 */	{KEY_F8, KEY_F8},   {KEY_F9, KEY_F9},
	/* 68 */	{KEY_F10, KEY_F10}, {0, 0},
	/* 70 */	{0, 0},             {KEY_HOME, '7'},
	/* 72 */	{KEY_UP, '8'},      {KEY_PGUP, '9'},
	/* 74 */	{'-', 0},           {KEY_LEFT, '4'},
	/* 76 */	{'5', 0},           {KEY_RIGHT, '6'},
	/* 78 */	{'+', 0},           {KEY_END, '1'},
	/* 80 */	{KEY_DOWN, '2'},    {KEY_PGDN, '3'},
	/* 82 */	{KEY_INS, '0'},     {KEY_DEL, KEY_DEL},
	/* 84 */	{KEY_F11, KEY_F11}, {KEY_F12, KEY_F12},
	/* 86 */	{'\\', '|'},        {'\n', '\n'},
	/* 88 */	{CTRL, 0},          {'/', 0},
	/* 90 */	{SYSRQ, PSCREEN},   {ALT, 0},
	/* 92 */	{KEY_HOME, 0},      {KEY_UP, 0},
	/* 94 */	{KEY_PGUP, 0},      {KEY_LEFT, 0},
	/* 96 */	{KEY_RIGHT, 0},     {KEY_END, 0},
	/* 98 */	{KEY_DOWN, 0},      {KEY_PGDN, 0},
	/* 100 */	{KEY_INS, 0},       {KEY_DEL, 0},
	/* 102 */	{KEY_WIN, 0},       {KEY_WIN, 0},
	/* 104 */	{KEY_MENU, 0}
};

using namespace myos;
using namespace myos::drivers;
using namespace myos::hardware;




KeyboardDriver::KeyboardDriver(InterruptManager* manager, uint8_t interruptLine)
: InterruptHandler(manager, interruptLine),
dataport(0x60),
commandport(0x64)
{
	this->interruptLine = interruptLine;
}

KeyboardDriver::~KeyboardDriver()
{

}

void KeyboardDriver::Activate()
{
	
	DEBUG("keyboard activate ");
	APIC::IO::MapLegacyIRQ(interruptLine - 0x20);
//	irq_enable(PIC_IRQ1);
	//APIC::IO::MapLegacyIRQ(0x1);
	while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xae); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xf4);


}

void KeyboardDriver::HandleInterrupt(isr_ctx_t *regs)
{
	//return ;
	keyboard_callback();
	//uint8_t scancode = inp(KEYBOARD_DATA_PORT);
	//pic_acknowledge(PIC_IRQ1);

}

void KeyboardDriver::keyboard_callback()
{
	//DEBUG("kasjndsddj");
	uint8_t scancode = inp(KEYBOARD_DATA_PORT);
	uint8_t code;
	uint32_t packet = 0;
	message_t msg;


	code = (uint8_t)convert(scancode);
	DEBUG("%c", code);
	if (code) {
		if (scancode & KEY_RELEASED) {
		packet = 0;
		msg.message = MESSAGE_KEY_RELEASE; // if released reset old (in case if user taps multiple times)
		}
		else {
		packet = 1;
		msg.message = MESSAGE_KEY_PRESS;
		}
		packet <<= 8;
		packet |= code;
	}

	msg.key = code;

	if (msg.message != old_msg.message || msg.key != old_msg.key) {
		window_add_message_to_focused(&msg);
		old_msg = msg;
	}
	//DEBUG("kasjndsddj\n");
}

int32_t KeyboardDriver::convert(uint32_t code) {


	static uint8_t k_shift = 0;

	static uint8_t k_num    = 0;
	static uint8_t k_caps   = 0;
	static uint8_t k_scroll = 0;

   
	uint32_t  key   = 0;
	uint8_t leds = false;
	uint8_t  i;

	if (code>=KEYMAP_SIZE){
		return 0;
	}

	if (code & KEY_RELEASED){
		code &= 0x7F;
		switch(code){
			case LSHIFT:
			case RSHIFT:
				k_shift = 0;
			return 0;
			case ALT:
				// k_alt   = 0;
			return 0;
			case CTRL:
				// k_ctrl  = 0;
			default:
				break; // modified
		}

    if (k_shift){
      key  = keymap[code][1];
    }
    else{
      key  = keymap[code][0];
    }
	}


	switch(code){
		case KEY_NUM_LOCK:
			k_num = !(k_num);
			leds = true;
		break;
		case KEY_SCROLL_LOCK:
			k_scroll = !(k_scroll);
			leds = true;
		break;
		case KEY_CAPS_LOCK:
			k_caps = !(k_caps);
			leds = true;
		break;
		case LSHIFT:
		case RSHIFT:
			k_shift = 1;
			return 0;
		case ALT:
			// k_alt  = 1;
			return 0;
		case CTRL:
		// k_ctrl = 1;
		default:
			if (k_shift){
				key  = keymap[code][1];
			}
			else{
				key  = keymap[code][0];
			}
		break;
	}

	if (leds){
		outp(0x60, 0xED);
		i = 0;
		if (k_scroll){
			i |= 1;
		}
		if (k_num){
			i |= 2;
		}
		if (k_caps){
			i |= 4;
		}
		outp(0x60, i);

		return 0;
	}

	return key;
}