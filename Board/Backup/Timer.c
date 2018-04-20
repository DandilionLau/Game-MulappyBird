/*
#include "stm32f10x.h"
#include "Clock.h"
#include "LED.h"
#include "Global.h"
#include "NVIC.H"

void IERG3810_TIM3_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR|=1<<1; //RIM3 is enable
    TIM3->ARR=arr;      // the clock will count form 0 to arr, then reset to be 0
    TIM3->PSC=psc;       //The counter clock frequency CK_CNT is equal to fC / (PSC[15:0] + 1)
    TIM3->DIER=1<<0;    // Update interrupt enabled.
    TIM3->CR1|=0x01;    // Counter enabled
    NVIC->IP[29]=0x45;
    NVIC->ISER[0]|=(1<<29);
}

void TIM3_IRQHandler(void)
{
    if(TIM3->SR & 1<<0)
		{
			
		}
		TIM3->SR&= ~(1<<0);//Update interrupt pending. This bit is set by hardware when the registers are updated:
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

void TIM4_IRQHandler(void)
{
    if(TIM4->SR & 1<<0)
     {
     // do something
     }  
    TIM4->SR&= ~(1<<0);//Update interrupt pending. This bit is set by hardware when the registers are updated:
}

void IERG3810_SYSTICK_Init10ms(void)
{
    SysTick->CTRL= 0; // Clear Systick->CTRL setting
    SysTick->LOAD= 89999;// no sure,  
   // CLKSOURCE=0: STCLK (FCLK/8)
    SysTick->CTRL |= 0x00000003;
}

void IERG3810_TIM3_PwmInit(u16 arr, u16 psc)// need modifying
{
    RCC->APB2ENR |=1<<3;     //PB5
    GPIOB->CRL&=0xFF0FFFFF;
    GPIOB->CRL|=0x00B0000;
    RCC->APB2ENR|=1<<0;
    AFIO->MAPR&=0xFFFFF3FF;
    AFIO->MAPR|=1<<11;//TIM3 channel is  Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)
    RCC->APB1ENR|=1<<1;// TIM3 timer clock enable
    TIM3->ARR=arr;
    TIM3->PSC=psc;
    TIM3->CCMR1|=7<<12;//Output compare 2 mode
    TIM3->CCMR1|=1<<11;//Output compare 2 preload enable
    TIM3->CCER|=1<<4;//Capture/Compare 2 output enable, OC2 signal is output on the corresponding output pin.
    TIM3->CR1=0x0080;// TIMx_ARR register is buffered
    TIM3->CR1|=0x01;// Counter enabled
}*/
