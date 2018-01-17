#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_LED.h"
#include "IERG3810_USART.h"

u32 sheep= 0;
int timeout= 20000;
int ps2count= 0;
int canPrint = 0;
int store[8];
u8 ps2key=0x00;


void IERG3810_PS2key_ExtiInit(void)
{
	RCC->APB2ENR |= 1 << 4;
	GPIOC->CRH&=0xFFFF00FF;
	GPIOC->CRH|=0x00008800;
	GPIOE->ODR|=0<<11;
	GPIOE->ODR|=0<<10;
	
	RCC->APB2ENR|=0x01;
	AFIO->EXTICR[2]&=0xFFFF0FFF;
  AFIO->EXTICR[2]|=0x00002000;
	EXTI->IMR|= 1<<11;
	EXTI->FTSR|= 1<<11;
	NVIC->IP[40]= 0x95;
	NVIC->ISER[1]|= (1<<8);
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
    //EXTI->RTSR|=1<<2;
    
    NVIC->IP[8]=0x75;
    NVIC->ISER[0]|=(1<<6);
}

/*
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
    //EXTI->RTSR|=1<<2;
    
    NVIC->IP[8]=0x65;
    NVIC->ISER[0]|=(1<<8);
}
*/
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

void EXTI15_10_IRQHandler(void)
{
	int x;
	Delay(10);
	
	if(ps2count >= 1 && ps2count <= 8)
	{
			uint32_t temp = GPIOC->IDR;
			temp &= 0x00000400;
			if( temp == 0)
			{
				store[ps2count-1] = 0;
			}
			else
			{
				store[ps2count-1] = 1;
			}
	}
	
	ps2count++;
	
	if(ps2count >= 11)
	{
		ps2count = 0;
		canPrint = 1;
	}
	
	EXTI->PR = 1 << 11;
}
/*
void EXTI2_IRQHandler(void)
{
    u8 i;
   // do something
    EXTI->PR=1<<2;  //Clear this exception pending bit
}

*/
void EXTIUP_IRQHandler(void)
{
    u8 i;
    //do something
    EXTI->PR=1<<2;  //Clear this exception pending bit
}