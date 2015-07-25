#ifndef _speed_h_
#define _speed_h_
 
void speed_init();
//计数一次
extern void speed_count();
//需要每400ms调用一次
extern void speed_CountList_addData();

#endif
