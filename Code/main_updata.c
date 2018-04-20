#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_LED.h"
#include "Global.h"
#include "NVIC.H"
#include "IERG3810_LCD.h"
#include "IERG3810_draw.h"
#include "IERG3810_timer.h"
#include "IERG3810_key_exit.h"

int b_x,b_y,w_x,w_y_up,w_y_low;
int wall_flag;
int game_process;
int score;
int wall_length;
int upEnable;

void gameData_init()
{
    b_x=110;
    b_y=150;
    w_x=270;//the location of walls is beyond the screen,now.
    w_y_up=240;
    w_y_low=100;
    wall_flag=0;
    score=0;
    game_process=0;
    wall_length=30;
    upEnable=0;
}

void draw_playing()
{
  draw_play_bg();
  draw_bird(b_x,b_y);
  draw_wall(w_x,w_y_up,w_y_low,wall_length);
  draw_play_score(score);
}

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

void update_flag()
{
	if(w_x<240 & w_x+wall_length>0)
		wall_flag=1;
	else{
		wall_flag=0;
		w_x=270;		//again, the walls are beyond the screen.
	} 
}

void check_process()
{
 if(b_x>w_x & b_x<w_x+wall_length &(b_y+5>w_y_up | b_y<w_y_low ))
	{
		process=2;
		TIM3->CR1|=0x00;   
	}
	else if(b_x==w_x+wall_length & (b_y<w_y_up & b_y>w_y_low))
	{
	  score++;
	}
	else;
}

void TIM3_IRQHandler()
{
    if(TIM3->SR & 1<<0)
		{
		upEnable=1;
		update_flag();
		check_process();
		if(b_y>0)
			b_y-=1;
		if(wall_flag==1)	
			w_x-=1;
		}
		TIM3->SR&= ~(1<<0);//Update interrupt pending. This bit is set by hardware when the registers are updated:
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
    //EXTI->RTSR|=1<<2;
    
    NVIC->IP[8]=0x65;
    NVIC->ISER[0]|=(1<<8);
}

void EXTI2_IRQHandler(void)
{
    u8 i;
   if(upEnable==1)
   b_y+=3;
   upEnable=0;
    EXTI->PR=1<<2;  //Clear this exception pending bit
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

void EXTIUP_IRQHandler(void)
{
    u8 i;
    if(game_process==0)
    {
    	TIM3->CR1|=0x01;   
    	game_process=1;
    }
    if(game_process=2)
    	game_process=0;
    EXTI->PR=1<<2;  //Clear this exception pending bit
}

int main(void)
{
	IERG3810_TFTLCD_Init();
	Delay(100000);
	IERG3810_clock_tree_init();
    IERG3810_LED_Init();
    IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_TIM3_Init(4999,7199);
	IERG3810_key2_ExtiInit();
	IERG3810_keyUP_ExtiInit(void);
	gameData_init();
	draw_start();
	while(1)
	{
	 if(game_process==0)
	 {
	  TIM3->CR1|=0x00;   
	 draw_start();
	 }
	 else if(game_process==1)
	 {
	  draw_playing();
	 }
	 else
	 {
	   draw_game_over();
	 }
	}
	
}
