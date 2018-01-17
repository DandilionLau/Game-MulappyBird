#ifndef __IERG3810_TOUCHSCREEN_H
#define __IERG3810_TOUCHSCREEN_H
#include "stm32f10x.h"



void IERG3810_TouchScreen_init(void);
void TouchScreenMOSI(u8 i);
void TouchScreenCS(u8 i);
void TouchScreenSckPulse(void);

u16 TouchScreenReadData(u8);   //read data from the tochscreen, input:1=Y, 3=Z1, 4=Z2, 5=X



#endif
