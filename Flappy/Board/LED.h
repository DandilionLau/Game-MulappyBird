#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"

// put procedure header here

void IERG3810_LED_Init(void)
{
	//LED-0
	RCC->APB2ENR|=1<<3;
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000;

	//LED-1
	RCC->APB2ENR|=1<<6;
	GPIOE->CRL &= 0xFF0FFFFF;
	GPIOE->CRL |= 0x00300000;
}

#endif
