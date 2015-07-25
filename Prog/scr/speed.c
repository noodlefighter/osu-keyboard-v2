#include <avr/io.h>
#include <avr/pgmspace.h>
#include "RGBLED.h"
#include "WorkMode.h"
//计速
#define speed_CountList_Length 3	//400ms*3=3.2Sec

volatile uint8_t speed_CountList[speed_CountList_Length]={0,0,0};  //循环队列，每400ms一个数据
volatile uint8_t speed_CountListNow=0;                  //循环队列当前位置
volatile uint8_t speed_CountListTotal=0;                 //队列中数量总和
volatile uint8_t speed_Data=0;                          //当前时间段按键次数统计 400ms内的数据

void speed_init(){
/*
	uint8_t i=speed_CountList_Length;
	while(i--)
		speed_CountList[i] = 0;
	
	speed_CountListNow = 0;
	speed_CountListTotal = 0;
	speed_Data = 0;
*/
}

void speed_count(){
	speed_Data++;
	workMode = WorkMode_SpeedTest;
}

//将speed_Data加入队列,进行计算，并将计算结果显示出来
volatile uint8_t unusedCount = 0;	//15s闲置 当数到38时闲置

void speed_CountList_addData(){
	speed_CountListTotal -= speed_CountList[speed_CountListNow];
	speed_CountListTotal += speed_Data;

	speed_CountList[speed_CountListNow] = speed_Data;
	speed_Data = 0;
 
 	speed_CountListNow++;
    if(speed_CountListNow >= speed_CountList_Length)
        speed_CountListNow = 0;

//display
/* 颜色表 （尽可能多一点） 只有非闲置状态才显示颜色（闲置:15s无按键即闲置,闲置时呼吸灯或无灯）
 * 序号 BPM      	颜色     折算1.2Sec打击次数	
 * 0    0-25     	暗蓝     2						
 * 1    25-60       淡蓝     5					
 * 2    60-110      亮蓝     8						
 * 3    110-125     暗黄     10
 * 4    125-150     亮橙     12				
 * 5    170-175     粉红     14
 * 6    175-200     大红     16
 * 7    200-225     紫红     18					
 * 8    225-250     深紫     20					
 * 9    250-275     白偏紫   22					
 * 10   275以上     金       22以上
 */


    uint8_t bpmLevel = 0,lightLevel = 10;
    if(speed_CountListTotal <= 2){
        bpmLevel = 0;
    }else if(speed_CountListTotal <=5){
        bpmLevel = 1;
    }else if(speed_CountListTotal <=8){
        bpmLevel = 2;
    }else if(speed_CountListTotal <=10){
        bpmLevel = 3;
    }else if(speed_CountListTotal <=12){
        bpmLevel = 4;
    }else if(speed_CountListTotal <=14){
        bpmLevel = 5;
		lightLevel = 15;
    }else if(speed_CountListTotal <=16){
        bpmLevel = 6;
		lightLevel = 20;
    }else if(speed_CountListTotal <=18){
        bpmLevel = 7;
		lightLevel = 26;
    }else if(speed_CountListTotal <=20){
        bpmLevel = 8;
		lightLevel = 32;
    }else if(speed_CountListTotal <=22){
        bpmLevel = 9;
		lightLevel =32;
    }else{
        bpmLevel = 10;
		lightLevel =32;
    }

    RGB_setColor(2 + bpmLevel,lightLevel);	//速度表示色 的色表偏移量为16

	if(bpmLevel==0){
		unusedCount++;
		if(unusedCount>=38){
			unusedCount=0;
			WorkMode_set(WorkMode_Unused);
		}
	}else{
		unusedCount=0;
	}

}
