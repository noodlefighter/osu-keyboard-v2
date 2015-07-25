#include <avr/io.h>
#include <avr/pgmspace.h>
#include "softPWM.h"

//软件PWM
/*	
 * 相关硬件资源
 * 0	PD3		KEY_LED1
 * 1	PD4		KEY_LED2
 * 2	PC3		G
 * 3	PC4		R
 * 4	PC5		B
*/

#define PWM_LED_ON(_v)  *PWM_PORT[_v] &= ~(1<<PWM_PORT_IO[_v])
#define PWM_LED_OFF(_v) *PWM_PORT[_v] |= (1<<PWM_PORT_IO[_v])

uint8_t PWM_timeLable = 0;

uint8_t *PWM_PORT[PWM_PORT_COUNT]={&PORTD,&PORTD,&PORTC,&PORTC,&PORTC};
uint8_t const PWM_PORT_IO[PWM_PORT_COUNT] ={4,3,3,4,5};
//pwm软件端口模式：0直接跳变 1跳变时渐入
//todo:为了省空间，把跳变和渐入模式取消了，统一成渐入
/*
#define PWM_PORT_Mode_Once 0
#define PWM_PORT_Mode_Shade 1
uint8_t const PWM_PORT_Mode[PWM_PORT_COUNT] = {1,1,1,1,1};
*/
//PWM_PORT_Mode_Shade
volatile uint8_t PWM_PORT_Shade_goal[PWM_PORT_COUNT]={0,0,0,0,0};
volatile uint8_t PWM_shadeTimeLable=0;
volatile uint8_t PWM_outputLevel[PWM_PORT_COUNT]={0,0,0,0,0};

void PWM_init(){
	PWM_timeLable = 0;
	PWM_shadeTimeLable = 0;

	uint8_t i=PWM_PORT_COUNT;
	while(i--){
		PWM_PORT_Shade_goal[i] = 0;
		PWM_outputLevel[i] = 0;
	}
}

void PWM_setOutputLevel(uint8_t n,uint8_t level){
	PWM_PORT_Shade_goal[n] = level;
/*//todo:为了省空间，把跳变和渐入模式取消了，统一成渐入
	if(PWM_PORT_Mode[n]==PWM_PORT_Mode_Once){
		PWM_outputLevel[n] = level;
	}
	else if(PWM_PORT_Mode[n]==PWM_PORT_Mode_Shade){
		PWM_PORT_Shade_goal[n] = level;
	}
*/
}

void PWM_setOutputLevel_direct(uint8_t n,uint8_t level){
	PWM_PORT_Shade_goal[n] = level;
	PWM_outputLevel[n] = level;
}

void PWM_Generator(){
	uint8_t i;
	for(i=0;i<PWM_PORT_COUNT;i++){
		if(PWM_timeLable!=0 && (PWM_timeLable <= PWM_outputLevel[i])){
			//输出1
			PWM_LED_ON(i);
		}	
		else{
			//输出0
			PWM_LED_OFF(i);
		}
	}

	PWM_shadeTimeLable++;
	if(PWM_shadeTimeLable>PWM_ShadeTimeLableMax){
		PWM_shadeTimeLable = 0;
		for(i=0;i<PWM_PORT_COUNT;i++){
			if(PWM_outputLevel[i] == PWM_PORT_Shade_goal[i]){
				continue;
			}						
			else if(PWM_outputLevel[i] < PWM_PORT_Shade_goal[i]){			
				PWM_outputLevel[i]++;
			}else if(PWM_outputLevel[i] > PWM_PORT_Shade_goal[i]){					
				PWM_outputLevel[i]--;
			}
		/*//todo:为了省空间，把跳变和渐入模式取消了，统一成渐入
			if(PWM_PORT_Mode[i]==PWM_PORT_Mode_Shade){
				if(PWM_outputLevel[i] == PWM_PORT_Shade_goal[i]){
					continue;
				}						
				else if(PWM_outputLevel[i] < PWM_PORT_Shade_goal[i]){			
					PWM_outputLevel[i]++;
				}else if(PWM_outputLevel[i] > PWM_PORT_Shade_goal[i]){					
					PWM_outputLevel[i]--;
				}
			}
		*/
		}
	}


	PWM_timeLable++;
	if(PWM_timeLable>PWM_TotalLevel){
		PWM_timeLable = 0;
	}
}
