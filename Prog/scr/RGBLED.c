#include <avr/io.h>
#include <avr/pgmspace.h>
#include "RGBLED.h"
#include "softPWM.h"

#define RGBLED_ColorQuantity 13

//RGB变量                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        

uint8_t const RGBLED_ColorTable[RGBLED_ColorQuantity][3] ={	//色表 ， 数值为比例, 255为最亮
//闲时颜色表
{0,0,0},			//0
{255,255,255},		//

//速度色表
{50,75,190},		//暗蓝 3
{50,75,190},		//淡蓝
{10,10,190},		//淡蓝
{80,80,30},			//暗黄
{127,50,0},			//亮橙
{255,128,255},		//粉红
{255,0,0},			//大红
{255,40,100},		//紫红
{141,20,187},		//深紫
{255,230,255},		//白偏紫
{255,201,0}			//金
};

//此函数显示的RGB灯，亮度只和level有关
//RGB灯PWM等级总量(allPWM) = 3*PWM_Total*(RGBLv/RGB_Total)
//PWM等级单一颜色分量R = RGB灯PWM等级总量*(R/R+G+B)

void RGB_setColor(uint8_t n,uint8_t level){
	uint16_t r,g,b,allRGB,allPWM;

	if (n>=RGBLED_ColorQuantity) return;

	r=RGBLED_ColorTable[n][0];
	g=RGBLED_ColorTable[n][1];
	b=RGBLED_ColorTable[n][2]; 
	
	allPWM = 3 * PWM_TotalLevel * level / RGB_TotalLevel;
	allRGB = r+g+b;
	
	r=(allPWM * r / allRGB);
	g=(allPWM * g / allRGB);
	b=(allPWM * b / allRGB);
	
	if(r>255){
		g=g*255/r;
		b=b*255/r;
		r=255;
	}else if(g>255){
		r=r*255/g;
		b=b*255/g;
		g=255;		
	}else if(b>255){
		r=r*255/b;
		g=g*255/b;
		b=255;		
	}
	
	PWM_setOutputLevel(RGB_rP,(uint8_t)r);
	PWM_setOutputLevel(RGB_gP,(uint8_t)g);
	PWM_setOutputLevel(RGB_bP,(uint8_t)b);
}
