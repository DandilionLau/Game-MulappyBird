#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include "stm32f10x.h"
/*
u32 sheep= 0;
int timeout= 20000;
int ps2count= 0;
int canPrint = 0;
int store[8];
u8 ps2key=0x00;
*/
void IERG3810_key2_ExtiInit(void);
void EXTI2_IRQHandler(void);
void IERG3810_NVIC_SetPriorityGroup(u8);
void IERG3810_PS2key_ExtiInit(void);
void IERG3810_keyUP_ExtiInit(void);
void IERG3810_key1_ExtiInit(void);
void IERG3810_key2_ExtiInit(void);
void IERG3810_TIM4_Init(u16, u16);
void TIM4_IRQHandler(void);

#endif
