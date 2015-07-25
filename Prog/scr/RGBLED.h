#ifndef _RGBLED_h_
#define _RGBLED_h_

#define RGB_TotalLevel 32
#define RGB_rP 3
#define RGB_gP 2
#define RGB_bP 4

//根据色表置RGB颜色，n为编号,level为明暗程度
extern void RGB_setColor(uint8_t n,uint8_t level);

#endif
