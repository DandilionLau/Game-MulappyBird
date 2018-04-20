#ifndef _IERG3180_TIMER.H
#define _IERG3180_TIMER.H
#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_LED.h"
#include "Global.h"
#include "NVIC.H"

//void IERG3810_TIM3_Init(u16, u16);
//void TIM3_IRQHandler(void);
void IERG3810_TIM4_Init(u16, u16);
void TIM4_IRQHandler(void);
//void IERG3810_SYSTICK_Init10ms(void);
//void IERG3810_TIM3_PwmInit(u16, u16);
#endif