//工作模式设置器

#ifndef _WORKMODE_h_
#define _WORKMODE_h_

#define WorkMode_Unused 	0 		//闲置
#define WorkMode_SpeedTest 	1 		//计速器模式
#define WorkMode_Setup 		2 		//设置模式，作用暂时为颜色展示，2后恢复为模式0

extern volatile uint8_t workMode;
void WorkMode_set(uint8_t mode);

void WorkMode_CLK();	//需要1ms调用一次

#endif
 
