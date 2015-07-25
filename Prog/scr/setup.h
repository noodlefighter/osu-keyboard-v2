//配置相关,需要最先init()
#ifndef _SETUP_H_
#define _SETUP_H_

//eeprom设置项
//闲置时背景色
#define Setup_bgColor_max 12
#define Setup_Address_bgColor (uint8_t*)0x01
//#define Setup_Default_bgColor 0x00
extern uint8_t Setup_bgColor;


//按键12的定义
#define Setup_key12Mode_max 6
#define Setup_Address_key12Mode (uint8_t*)0x02
extern uint8_t Setup_key12Mode;

//按键3的定义
#define Setup_key3Mode_max 6
#define Setup_Address_key3Mode (uint8_t*)0x03
extern uint8_t Setup_key3Mode;

//JP设置项
#define Setup_key12LED_OftenOff	0
#define Setup_key12LED_Always 	1
#define Setup_key12LED_OftenOn	2
#define Setup_key12LED_Never 	3
extern uint8_t Setup_key12LED;

void Setup_init();
//add是ee的地址 var是变量地址 max是循环上限
void Setup_setLoop(uint8_t* add,uint8_t* var,uint8_t max);

#endif
