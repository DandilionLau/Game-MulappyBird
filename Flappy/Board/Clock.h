#ifndef __IERG3810_CLOCK_H
#define __IERG3810_CLOCK_H
#include "stm32f10x.h"

void IERG3810_clock_tree_init(void)
{
    u8 PLL=7;    
    unsigned char temp=0;
    RCC->CFGR &= 0xF8FF0000; 
		RCC->CR &= 0xFEF6FFFF;
		RCC-> CR|= 0x00010000; // Turn on the HSE oscillator 
    while(!(RCC->CR>>17));  // Check the HSERDY
    RCC->CFGR=0x00000400; // Set the PPRE1 to be 100
    RCC->CFGR|=PLL<<18; // Set PLL multiplexer to be 111
		RCC->CFGR|=1<<16; // Set the PLLSRC to be 1
		FLASH->ACR|=0x32; // Set FLASH with 2 wait states
    RCC->CR|=0x01000000; // Set PLL as ON
    while(!(RCC->CR>>25)); // Check PLLRDY
    RCC->CFGR|=0x00000002; // Set SW as 10
    while(temp!=0x02) // Check SWS
    {
    temp=RCC->CFGR>>2;
    temp&=0x03;
    }
}

#endif
