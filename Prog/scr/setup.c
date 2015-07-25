/*设置方法:
	跳线：映射

	三个键一起按:设置bt3自定义按键的值
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h> 
#include <avr/interrupt.h>
#include "KeyScan.h"
#include "setup.h"
#include "softPWM.h"
#include "workmode.h"
//JP对程序进行设置
/*
#define JP1 (!(PIND && (1<<5)))
#define JP2 (!(PIND && (1<<6)))
#define JP3 (!(PIND && (1<<7)))
#define JP4 (!(PINB && (1<<0)))
#define JP5 (!(PINB && (1<<1)))
#define JP6 (!(PINB && (1<<2)))
*/
//全局变量
uint8_t Setup_bgColor;
uint8_t Setup_key12Mode;
uint8_t Setup_key3Mode;
uint8_t Setup_scanMode;
uint8_t Setup_key12LED;
//局部变量
uint8_t needEE=0;
uint8_t *addEE;
uint8_t dataEE;


void Setup_init(){
/*
 * PD5-7	JP1-JP3
 * PB0-2	JP4-JP6	

		JP2	JP1	按键光
		0	0	常亮
		0	1	常暗，按下变亮
		1	0	常亮，按下变暗
		1	1	不亮
		
		JP6
		0		华丽模式
		1		纯键盘模式
*/

/*//todo 省空间,只用JP1 JP2
 	uint8_t jpData;
	jpData = ~(((PIND & 0xe0)>>5)|((PINB & 0x07)<<3));
	Setup_key12LED = jpData & 0x03;
*/
	Setup_key12LED = (~PIND & 0x60)>>5;

//read EEPROM
	eeprom_busy_wait();
	Setup_bgColor = eeprom_read_byte(Setup_Address_bgColor);
	Setup_key12Mode = eeprom_read_byte(Setup_Address_key12Mode);
	Setup_key3Mode = eeprom_read_byte(Setup_Address_key3Mode);
	
}


ISR(EE_READY_vect){
	if(needEE){
		eeprom_write_byte(addEE,dataEE);		
		needEE=0;
		EECR &= ~(1<<EERIE);
	}
}
/*
void Setup_save(){
//	eeprom_busy_wait();
//	eeprom_write_byte(Setup_Address_bgColor,Setup_bgColor);

//	eeprom_busy_wait();
//	eeprom_write_byte(Setup_Address_key3Mode,Setup_key3Mode);
}


void Setup_settingMode(){
//rgb灯于闲置时的颜色：RGB灯无效/RGB闲时不亮/米黄/暖红/艳紫/亮蓝/暖橙/瞎眼白
//按键灯模式：闲时呼吸灯/常亮/按下才亮，渐暗/按下就灭 ，渐亮
	
}
*/



//add是ee的地址 num是循环上限
void Setup_setLoop(uint8_t* add,uint8_t* var,uint8_t max){
	addEE = add;
	(*var)++;
	if(*var>max){
		*var=0;
	}
	dataEE=*var;
	KeyScan_init();
	
	WorkMode_set(WorkMode_Setup);
	EECR |=(1<<EERIE);
	needEE=1;
}

