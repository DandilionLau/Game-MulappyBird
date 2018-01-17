#ifndef _USART_H
#define _USART_H
#include "stm32f10x.h"

// put procedure header here

void IERG3810_USART2_init(u32 pclk1,u32 bound)
{
    float temp;
    u16 mantissa;
    u16 fraction;
    temp=(float)(pclk1*1000000)/(bound*16);
    mantissa=temp;
    fraction=(temp-mantissa)*16;
    mantissa<<=4;
    mantissa+=fraction;
    RCC->APB2ENR|=1<<2; // Enable APB2, set APB2ENR with 1 at PIN2
    RCC->APB1ENR|=1<<17; // Enable APB1, set APB1ENR with 1 at PIN17
    GPIOA->CRL&=0xFFFF00FF; // Set PA2, PA3 Alternate Function
    GPIOA->CRL|=0x00008B00; // Set PA2, PA3 Alternate Function
    RCC->APB1RSTR|=1<<17; // Set USART2RST at APB1RSTR PIN17
    RCC->APB1RSTR&=~(1<<17); 
    USART2->BRR=mantissa; // Set the baud rate register
    USART2->CR1|=0x2008; // Set the control register
}

void IERG3810_USART1_init(u32 pclk1,u32 bound)
{
    float temp;
    u16 mantissa;
    u16 fraction;
    temp=(float)(pclk1*1000000)/(bound*16);
    mantissa=temp;
    fraction=(temp-mantissa)*16;
    mantissa<<=4;
    mantissa+=fraction;
    RCC->APB2ENR|=1<<2;
    RCC->APB2ENR|=1<<14; // Change PIN to 14
    GPIOA->CRH&=0xFFFFF00F; // Change the bits for configuration
    GPIOA->CRH|=0x000008B0;
    RCC->APB2RSTR|=1<<14;
    RCC->APB2RSTR&=~(1<<14);
    USART2->BRR=mantissa; // Set the baud rate register
    USART2->CR1|=0x2008; // Set the control register
}

#endif
