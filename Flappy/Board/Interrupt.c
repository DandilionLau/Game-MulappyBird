#include "stm32f10x.h"

void IERG3810_PS2key_ExtiInit(void)
{
	RCC->APB2ENR|=1<<4;
	GPIOC->CRH &= 0xFFFF0FFF;
	GPIOC->CRH |= 0x00008000;
	GPIOC->ODR |= 1<<11;
	
	
	RCC->APB2ENR |= 0x01;
	AFIO->EXTICR[2] &= 0xFFFF0FFF; // p185
	AFIO->EXTICR[2] |= 0x00002000;
	EXTI->IMR |= 1<<11;
	EXTI->FTSR |=1<<11;

	NVIC->IP[40] = 0x95;
	NVIC->ISER[1] |= (1<<8);
	
	//pc10 data
	RCC->APB2ENR|=1<<4;
	GPIOC->CRH &= 0xFFFFF0FF;
	GPIOC->CRH |= 0x00000800;
}

void IERG3810_keyUP_ExtiInit(void)
{
    RCC->APB2ENR|=1<<2;       //pot A 0
    GPIOE->CRL&=0xFFFFFFF0;
    GPIOE->CRL|=0x00000008;
    GPIOE->ODR|=1<<0;
    RCC->APB2ENR|=0x01;
    AFIO->EXTICR[0]&=0xFFFFFFF0;
    AFIO->EXTICR[0]|=0x00000004;//set pinE as input source
    EXTI->IMR|=1<<0;
    EXTI->FTSR|=1<<0;
    
    NVIC->IP[8]=0x75;
    NVIC->ISER[0]|=(1<<6);
}

void IERG3810_key2_ExtiInit(void)
{
    RCC->APB2ENR|=1<<6;//port E 2
    GPIOE->CRL&=0xFFFFF0FF;
    GPIOE->CRL|=0x00000800;
    GPIOE->ODR|=1<<2;
    RCC->APB2ENR|=0x01;
    AFIO->EXTICR[0]&=0xFFFFF0FF;
    AFIO->EXTICR[0]|=0x00000400;//set pinE as input source
    EXTI->IMR|=1<<2;//event masked on line x
    EXTI->FTSR|=1<<2;//falling trigger configuration
    
    NVIC->IP[8]=0x65;
    NVIC->ISER[0]|=(1<<8);
}

void IERG3810_key1_ExtiInit(void)
{
    RCC->APB2ENR|=1<<6;//port E 2
    GPIOE->CRL&=0xFFFF0FFF;
    GPIOE->CRL|=0x00008000;
    GPIOE->ODR|=1<<3;
    RCC->APB2ENR|=0x01;
    AFIO->EXTICR[0]&=0xFFFF0FFF;
    AFIO->EXTICR[0]|=0x00004000;//set pinE as input source
    EXTI->IMR|=1<<2;//event masked on line x
    EXTI->FTSR|=1<<2;//falling trigger configuration
    
    NVIC->IP[8]=0x65;
    NVIC->ISER[0]|=(1<<8);
}

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup)
{
    u32 temp,temp1;
    temp1=prigroup & 0x00000007;
    temp1<<=8;
    temp = SCB->AIRCR;
    temp &=0x0000F8FF;
    temp|=0x05FA0000;
    temp|=temp1;
    SCB->AIRCR=temp;
}

void IERG3810_TIM4_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR|=1<<2; //RIM4 is enable
    TIM4->ARR=arr;      // the clock will count form 0 to arr, then reset to be 0
    TIM4->PSC=psc;       //The counter clock frequency CK_CNT is equal to fC / (PSC[15:0] + 1)
    TIM4->DIER=1<<0;    // Update interrupt enabled.
    TIM4->CR1|=0x01;    // Counter enabled
    NVIC->IP[30]=0x45;
    NVIC->ISER[0]|=(1<<30);
}

