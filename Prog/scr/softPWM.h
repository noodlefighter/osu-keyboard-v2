#ifndef _softPWM_h_
#define _softPWM_h_

//pwm端口数
#define PWM_PORT_COUNT 5
//pwm分级数
#define PWM_TotalLevel 64
//渐变模式渐变速度（PWM_shadeTimeLable的最大值）
//todo: 为了压缩程序空间，把时标变量改成了8位，有空间了就改回来
#define PWM_ShadeTimeLableMax 80
extern void PWM_init();
//需要循环调用此函数
extern void PWM_Generator(void);
//改变输出等级
extern void PWM_setOutputLevel(uint8_t n,uint8_t level);
extern void PWM_setOutputLevel_direct(uint8_t n,uint8_t level);
#endif
