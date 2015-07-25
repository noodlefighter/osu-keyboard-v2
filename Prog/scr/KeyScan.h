#ifndef _KEYSCAN_H_
#define _KEYSCAN_H_

#define KEYamount	3


#define KEY_C       6
#define KEY_Z       29
#define KEY_X       27
#define KEY_A       4
#define KEY_S       22
#define KEY_D       7
#define KEY_Q		0x14
#define KEY_W		0x1A
#define KEY_E		8

#define KEY_LEFT   0x50
#define KEY_DOWN	0x51
#define KEY_UP		0x52
#define KEY_RIGHT	0x4F

#define KEY_ESC		0x29

#define KEY_PAGEUP 0x4b
#define KEY_PAGEDOWN 0x4e

#define KEY_F1		58
#define KEY_F2		59
#define KEY_F8		65
#define KEY_F12		69
#define KEY_SPACE	0x2C
#define KEY_ENTER	0x28
//volatile uint8_t keyChanged();

extern volatile uint8_t  KeyScan_state[KEYamount];
extern uint8_t KeyScan_keyValue[KEYamount];
extern volatile uint8_t KeyScan_keyChanged;

void KeyScan_init();
void KeyScan();

#endif
 
