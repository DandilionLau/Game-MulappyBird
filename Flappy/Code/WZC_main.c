#include "stm32f10x.h"
#include "IERG3810_clock.h"
#include "IERG3810_USART.h"
#include "IERG3810_LED.h"
#include "GLOBAL.h"
#include "FONT.h"
#include "CFONT.h"
#include "lcd.h"
#include "stdlib.h"
#include "adcrand.h"


#define left 8
#define right 2
#define up 6
#define down 4
#define x_border 210
#define y_border 260

extern u8 m;
extern u8 s[220][270];
extern u8 x,y; //????


struct compx {u16 x,y;};
struct compx snake[200]; //????????
u8 m; //???
u8 s[220][270];
u8 flag1; //????????????????
u8 eatflag=1; //?????????
u8 keynum=right;//???
u8 prekeynum=right; //??????
u8 s1[]="GAME OVER";
u8 x,y; //?????????
u8 tailx,taily;//???????,????????
u8 highscore = 0;

void lcd_draw(void)// draw border
{
	u16 x_int=0,y_int=0;
	for(;y_int<y_border;y_int++)
	{
		IERG3810_TFTLCD_DrawDot(x_border,y_int, Green);
		
	}
	
	for(;x_int<x_border;x_int++)
	{
		IERG3810_TFTLCD_DrawDot(x_int,y_border, Green);
		
	}
}

void Snake_Init(void)
{
	u8 i;
	snake[0].x=snake[1].x=snake[2].x=10;
	snake[0].y=30;
	snake[1].y=20;
	snake[2].y=10;
	m=3;
	s[1][1]=s[1][2]=1; //??????,?????????
	for(i=0;i<=26;i++)
	{
		s[0][i*10]=1;
		s[210][i*10]=1;
	}
	for(i=0;i<=21;i++)
	{
		s[i*10][0]=1;
		s[i*10][260]=1;
	}
}


void food(void)
{
	u16 random;
	x=y=0;
	
	while(s[x][y]==1)
	{
		srand(temp());//using adc initial random
		random=rand();
		x=(random%20+1)*10; //x???1-20
		random=rand();
		y=(random%25+1)*10; //y???1-25
	}
	IERG3810_TFTLCD_FillRectangle(Red,  x,  y,   10,  10);
//	Draw_Grid(x,y,LCD_COLOR_GREEN);
	
}


//----------------------snake initial//



u16 update_rate = 50;//1s
// ps2 read parameters and functions
u16 input = 0;
u32 ps2count = 0;
u8 data=0;
u8 data1=0;
u8 data2=0;
char enter_char=' ';
char require_char='1';

char  test0[11] = {0,0,0,0,0,0,0,0} ;
char  reg[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
u8 j =0;


void IERG3810_PS2key_ExtiInit(void)
{
	//pc11 clock
	
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


void EXTI15_10_IRQHandler(void)
{
	uint32_t keys= GPIOC->IDR;
	
	keys &= (1<<10);

	if(keys == 0)
	{
		test0[j] = '0';
	}
	else 
	{	
		test0[j]= '1';
	}
	input = input << 1 ;
	j++;
	if(j == 11)
		j = 0;
	
	GPIOB->BRR = 1 << 5;
	GPIOE->BRR = 1 << 5;
	
	ps2count++;
  delay(10);
	
	EXTI->PR = 1<<11;
}


char ps2_decode(u8 ps2data_1,u8 ps2data_2)
{
		char temp =' ';
		if(ps2data_1 == 0xf0)
		{
			switch(ps2data_2)
			{
				case 0x70: temp = '0';break;
				case 0x69: temp = '1';break;
				case 0x72: temp = '2';break;
				case 0x7a: temp = '3';break;
				case 0x6b: temp = '4';break;
				case 0x73: temp = '5';break;
				case 0x74: temp = '6';break;
				case 0x6c: temp = '7';break;
				case 0x75: temp = '8';break;
				case 0x7d: temp = '9';break;
				case 0x5a: temp = 'e';break;  //enter
				case 0x71: temp = '.';break;  //	
				case 0x79: temp = '+';break;
				case 0x4a: temp = '/';break;
				case 0x7c: temp = '*';break;
				case 0x77: temp = 'n';break; //num lock
				case 0x66: temp = 'b';break; //back space
				case 0x7b: temp = '-';break;		
			}	
		}
		return temp;

}

//--------------------

//LCD------------



//------------------------


//usart----------------------------------
void USART_print(u8 USARTport, char  * st)
{
	u8 i=0;
	while (st[i] !=0x00)
	{
		if (USARTport == 1) USART1->DR = st[i];
		if (USARTport == 2) USART2->DR = st[i];
		//Delay(50000);
		if (USARTport == 1) while((USART1->SR&0x00000080) == 0);
		if (USARTport == 2) while((USART2->SR&0x00000080) == 0);

		i++;
	}
}
void USART_print_char(u8 USARTport, char  st)
{

		if (USARTport == 1) USART1->DR = st;
		if (USARTport == 2) USART2->DR = st;
		//Delay(50000);
		if (USARTport == 1) while((USART1->SR&0x00000080) == 0);
		if (USARTport == 2) while((USART2->SR&0x00000080) == 0);

}


void IERG3810_NVIC_SetPriorityGroup(u8 prigroup)
{
	u32 temp, temp1;
	temp1 = prigroup & 0x00000007;
	//temp1 = prigroup & 0x00000007;
	temp1 <<= 8; //"WHY?"
    temp = SCB->AIRCR;
    temp &= 0x0000F8FF;
    temp |= 0x05FA0000;
    temp |= temp1;
    SCB->AIRCR = temp;
}



void DS0_turnOff()
{
	GPIOB->BSRR =1 << 5;
}

void DS0_turnOff2()
{
	DS0_turnOff();
}



void IERG3810_SYSTICK_Init10ms(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 90000;
	
	SysTick->CTRL |= 0x00000003 ;
}

void print_score()
{
	u8 tmp = m;
	u8 bit1= 0,bit2=0;
	tmp = tmp - 0x03;
	bit1 = tmp % 0x0a;
	
	IERG3810_TFTLCD_FillRectangle(Black,  64,  270,   8,  16);//clear
	delay(10);
	IERG3810_TFTLCD_ShowChar(64,270,0x30+bit1,Green,Black);//0-9
	
	tmp =tmp - tmp % 0x0a;
	bit2 = tmp / 0x0a;
	IERG3810_TFTLCD_FillRectangle(Black,  56,  270,   8,  16);//clear
	delay(10);
	IERG3810_TFTLCD_ShowChar(56,270,0x30+bit2,Green,Black);//10-90
	
}

void initial_all_parameters()
{
			IERG3810_TFTLCD_FillRectangle(Black,  0,  0,   240,  320);
			lcd_draw();
			Snake_Init();
			food();
			prekeynum =	right;
			keynum =	right;
			enter_char = '6';
			update_rate = 50;
			two_second_timer=0;
			acti_timer=0;
			renew_timer=0;
			punishment_timer=0;
			acti_flag=0;
			IERG3810_TFTLCD_ShowChar(0,270,0x73,Green,Black);//s
			IERG3810_TFTLCD_ShowChar(8,270,0x63,Green,Black);//c
			IERG3810_TFTLCD_ShowChar(16,270,0x6f,Green,Black);//o
			IERG3810_TFTLCD_ShowChar(24,270,0x72,Green,Black);//r
			IERG3810_TFTLCD_ShowChar(32,270,0x65,Green,Black);//e
			IERG3810_TFTLCD_ShowChar(40,270,0x3A,Green,Black);//:
}

void open_image(u8 highscore)
{
		u8 tmp = highscore;
		u8 bit1= 0,bit2=0;
		IERG3810_TFTLCD_FillRectangle(Black,  0,  0,   240,  320);
		
		IERG3810_TFTLCD_ShowChar(50,160,0x68,Green,Black);//h
		IERG3810_TFTLCD_ShowChar(58,160,0x69,Green,Black);//i
		IERG3810_TFTLCD_ShowChar(66,160,0x67,Green,Black);//g
		IERG3810_TFTLCD_ShowChar(74,160,0x68,Green,Black);//h

		IERG3810_TFTLCD_ShowChar(90,160,0x73,Green,Black);//s
		IERG3810_TFTLCD_ShowChar(98,160,0x63,Green,Black);//c
		IERG3810_TFTLCD_ShowChar(106,160,0x6f,Green,Black);//o
		IERG3810_TFTLCD_ShowChar(114,160,0x72,Green,Black);//r
		IERG3810_TFTLCD_ShowChar(122,160,0x65,Green,Black);//e
		IERG3810_TFTLCD_ShowChar(130,160,0x3A,Green,Black);//:
	
		bit1 = tmp % 0x0a;
		tmp =tmp - tmp % 0x0a;
		bit2 = tmp / 0x0a;
		
		IERG3810_TFTLCD_ShowChar(138,160,0x30+bit2,Green,Black);//10-90
		IERG3810_TFTLCD_ShowChar(146,160,0x30+bit1,Green,Black);//0-9
		
}
int main(void)
{
	u8 k;
	u8 flag_usart= 0; //whether you enter a char

	char enter_tmp= ' '; 
	u8 i =0;
	u32 timeout_ps2 = 0;
	IERG3810_SYSTICK_Init10ms();
  IERG3810_clock_tree_init();
	IERG3810_LED_Init();
	IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_PS2key_ExtiInit();
	IERG3810_USART2_init(36,9600);
	IERG3810_USART1_init(72,9600);
	IERG3810_TFTLCD_Init();

	
	IERG3810_TFTLCD_FillRectangle(White,  0,  0,   240,  320);
	//initial part
	
	
	

  while(1)
	{
		if (start_flag == 0) //start
		{	
			USART_print(1,"it is start image");
			USART_print(1,"\n");
			enter_tmp = ' ';
			open_image(highscore);
			
			do{
				if(ps2count >= 11)
				{
					data=0;
					for(i = 1; i<9; i ++)
					{
						if(test0[i] ==  '1')
					{
						data |= 1<<(i-1);
					}
				}					
					
				enter_tmp = ps2_decode(data,data1);		

				data1 = data;
				input = input >>1;
				input &=0xFF;
	
				input = 0x00;
				ps2count=0x00;
				
				EXTI->PR = 1<<11;
				}
			}while(enter_tmp != 'n');
			
			start_flag = 1;
			
			//initial
			initial_all_parameters();
			print_score();
		}
		
		
		if(ps2count >= 11)
		{
				data=0;
				for(i = 1; i<9; i ++)
				{
					if(test0[i] ==  '1')
					{
						data |= 1<<(i-1);
					}
				}					
					
				enter_char = ps2_decode(data,data1);
				if(enter_char == require_char)
					flag_usart = 1;

				data1 = data;
				input = input >>1;
				input &=0xFF;

				input = 0x00;
				ps2count=0x00;
				
				EXTI->PR = 1<<11;
		}
		
	
		if (enter_char == '+') //pause
		{	
			USART_print(1,"it is pause");
			USART_print(1,"\n");
			pause_flag = 1;
			enter_tmp = ' ';
			do
			{
				if(ps2count >= 11)
				{
					data=0;
					for(i = 1; i<9; i ++)
					{
						if(test0[i] ==  '1')
					{
						data |= 1<<(i-1);
					}
				}					
					
				enter_tmp = ps2_decode(data,data1);
				
				data1 = data;
				input = input >>1;
				input &=0xFF;
	
				input = 0x00;
				ps2count=0x00;
				
				EXTI->PR = 1<<11;
				}
			}while(enter_tmp != '+');
			pause_flag=0;
			
			enter_char = ' ';
			
		}
		
		
		
		//speed up the speed		
		if(punishment_timer >= 1000)
		{
			punishment_timer = 0;
			if(update_rate >= 30)
				update_rate -=10;
			//reduce the update rate by 0.1s
		}
		

		if(renew_timer >= update_rate)
		{
			IERG3810_TFTLCD_FillRectangle(Black,  tailx,  taily,   10,  10);
			    if(eatflag==1)  
		    {
					s[tailx][taily]=0; 
				}
				eatflag=0;
				tailx=snake[m-1].x;
				taily=snake[m-1].y;
				for(k=m-1;k>0;k--)  
				{
					snake[k].x=snake[k-1].x;
					snake[k].y=snake[k-1].y;
				}
				switch(keynum)
				{
					case up:
					if(prekeynum==left||prekeynum==right||prekeynum==up) 
					{	
						snake[0].x-=10;
					break;}                                
					if(prekeynum==down)
					{snake[0].x+=10;keynum=down;break;}
					
					case down:
					if(prekeynum==left||prekeynum==right||prekeynum==down)
					{snake[0].x+=10;
					break;}
					if(prekeynum==up)
					{snake[0].x-=10;keynum=up;break;}
					
					case left:
					if(prekeynum==down||prekeynum==up||prekeynum==left)
					{snake[0].y-=10;
					break;}
					if(prekeynum==right)
					{snake[0].y+=10;keynum=right;break;}
					
					case right:
					if(prekeynum==down||prekeynum==up||prekeynum==right)
					{snake[0].y+=10;
					break;}
					if(prekeynum==left)
					{	snake[0].y-=10;keynum=left;break;}				
					default:break;
			}
			if(snake[0].x==x && snake[0].y==y)
			{
				eatflag=1;
			}
			flag1=1;
			renew_timer=0;
			
		}
		
		
		
		//randomly appear word by usart
		
		if(acti_timer >= 1500) //30s generate
		{
			acti_timer = 0;
			acti_flag = 1;
			USART_print(1,"Please enter in 2 seconds");
			USART_print(1,"\n");
			require_char = '1';
			USART_print_char(1,require_char);
			USART_print(1,"\n");//print random number
			
		}
		if((two_second_timer % 100) == 0 ) //light as time goes
		{
			GPIOB->ODR ^=1 << 5; 
			GPIOE->ODR ^=1 << 5; 
		}
		
		if(two_second_timer >=300) //3s at most
		{
			two_second_timer = 0;
			acti_flag=0;
			USART_print(1,"it is 3 seconds");
			USART_print(1,"\n");
			if(flag_usart == 0)
			{	
				start_flag = 0;
				//USART_print(1,"set false flag");
			}
			else
			{
				start_flag = 1;
				flag_usart = 0;
			}
		}
		
		if(flag1==1)
			{
				if(s[snake[0].x][snake[0].y]!=1)  
				{
					if(eatflag==0) //not get food
					{
					  IERG3810_TFTLCD_FillRectangle(Black,  tailx,  taily,   10,  10);
						s[tailx][taily]=0; 
						for(i=0;i<m;i++)
						{
						  IERG3810_TFTLCD_FillRectangle(Black,  tailx,  taily,   10,  10);//used for debug the tail problem 
							IERG3810_TFTLCD_FillRectangle(Yellow,  snake[i].x,  snake[i].y,   10,  10);
							s[snake[i].x][snake[i].y]=1;  
						}
					}
					else //get food
					{	
						print_score();						
						m++;
						print_score();
						for(i=0;i<m;i++)  //????????????,????????????
						{
							
							IERG3810_TFTLCD_FillRectangle(White,  snake[i].x,  snake[i].y,   10,  10);
							s[snake[i].x][snake[i].y]=1;  
						}
					food();  //???????
					IERG3810_TFTLCD_FillRectangle(Black,  tailx,  taily,   10,  10);
					IERG3810_TFTLCD_FillRectangle(Black,  0,  0,   10,  10);
					}
					prekeynum=keynum; //??????????prekeynum?
					flag1=0;
				}
        else if(s[snake[0].x][snake[0].y]==1)//game over
				{
					start_flag = 0;
					if((m-3)>highscore)
						highscore = (m-3);

				}
	
			
			}
		
				switch(enter_char)
			{
				case '4':keynum=left;break;
				case '6':keynum=right;break;
				case '8':keynum=up;break;
				case '2':keynum=down;break;
			}
		
			
	
	

			
			
	

		
		
		
	}
}
