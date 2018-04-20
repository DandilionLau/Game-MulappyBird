#include "IERG3810_TouchScreen.h"
#include "stm32f10x.h"
#include "Clock.h"
#include "LED.h"
#include "CFONT.H"
#include "Global.h"
#include "USART.H"
#include "Interrupt.h"
#include "stdlib.h"
#include "Bird.h"
#include "Wall.h"
#include "Logo.h"
#include "Num.h"
#include "FONT.H"

#define White		0xFFFF
#define Black		0x0000
#define Grey		0xF7DE
#define Blue		0x001F
#define LightBlue		0x4df9
#define Red			0xF800
#define Magenta	0xF81F
#define Green		0x07E0
#define Cyan		0x7FFF
#define Yellow	0xFFE0

#define LCD_LIGHT_ON GPIOB->ODR=1<<0
#define LCD_RD GPIOD->ODR=1<<4

#define LCD_BASE ((u32)(0x6C000000 |0x000007FE))
#define LCD	((LCD_TypeDef *) LCD_BASE)

int b_x,b_y; // X and Y position for bird
int canStart; // Decide whether to start the game
int status; // Game status
int touchXStatus; // Touch Status
int speed; // Game speed control
int Number; // Use to count down to zero when started
int ifBG; // To show if background has been drawn
char mode[3] = "umd";
int next; // Used to show the next shape of bird
int score; // Game score
int wallX[3];
int wallY[3];
int wallPass[3]; // Three set of arrays to store the X position/Y position/enable of wall

u32 sheep= 0; // parameters used for PS2 keyboard input
int timeout= 20000;
int ps2count= 0;
int canPrint = 0;
int store[8];
u8 ps2key=0x00; // store which key has been pressed

u16 OldXPosition;
u16 TsXposition; //TouchScreen  x-position 
u16 TsYposition; //TouchScreen y-position

void Delay(u32 count) // Delay function
{
		u32 i;
		for (i=0; i<count;i++);
}

typedef struct 
{
	u16 LCD_REG;
	u16 LCD_RAM;
}	LCD_TypeDef;

void IERG3810_TFTLCD_WrReg(u16 regval)
{
	LCD->LCD_REG=regval;
}

void IERG3810_TFTLCD_WrData(u16 data)
{
	LCD->LCD_RAM=data;
}

void IERG3810_TFTLCD_SetParameter(void) // Give the parameters for LCD
{
	IERG3810_TFTLCD_WrReg(0X01);
	IERG3810_TFTLCD_WrReg(0X11);
	
	IERG3810_TFTLCD_WrReg(0X3A);
	IERG3810_TFTLCD_WrData(0X55);
	
	IERG3810_TFTLCD_WrReg(0X29);
	IERG3810_TFTLCD_WrReg(0X36);
	IERG3810_TFTLCD_WrData(0XCA);
}


void IERG3810_TFTLCD_Init(void) // Initailization of LCD
{
	RCC->AHBENR|=1<<8;
	RCC->APB2ENR|=1<<3;
	RCC->APB2ENR|=1<<5;
	RCC->APB2ENR|=1<<6;
	RCC->APB2ENR|=1<<8;
	GPIOB->CRL&=0XFFFFFFF0;
	GPIOB->CRL|=0X00000003;
	
	GPIOD->CRH&=0X00FFF000;
	GPIOD->CRH|=0XBB000BBB;
	GPIOD->CRL&=0XFF00FF00;
	GPIOD->CRL|=0X00BB00BB;
	
	GPIOE->CRH&=0X00000000;
	GPIOE->CRH|=0XBBBBBBBB;
	GPIOE->CRL&=0X0FFFFFFF;
	GPIOE->CRL|=0XB0000000;
	
	GPIOG->CRH&=0XFFF0FFFF;
	GPIOG->CRH|=0X000B0000;
	GPIOG->CRL&=0XFFFFFFF0;
	GPIOG->CRL|=0X0000000B;
	
	FSMC_Bank1->BTCR[6]=0X00000000;
	FSMC_Bank1->BTCR[7]=0X00000000;
	FSMC_Bank1E->BWTR[6]=0X00000000;
	FSMC_Bank1->BTCR[6]|=1<<12;
	FSMC_Bank1->BTCR[6]|=1<<14;
	FSMC_Bank1->BTCR[6]|=1<<4;
	FSMC_Bank1->BTCR[7]|=0<<28;
	FSMC_Bank1->BTCR[7]|=1<<0;
	FSMC_Bank1->BTCR[7]|=0XF<<8;
	FSMC_Bank1E->BWTR[6]|=0<<28;
	FSMC_Bank1E->BWTR[6]|=0<<0;
	FSMC_Bank1E->BWTR[6]|=3<<8;
	FSMC_Bank1->BTCR[6]|=1<<0;
	IERG3810_TFTLCD_SetParameter();
	LCD_LIGHT_ON;
}


void IERG3810_TFTLCD_DrawDot(u16 x,u16 y, u16 color) // Draw a dot in the given position
{
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(x>>8);
	IERG3810_TFTLCD_WrData(x & 0xFF);
	IERG3810_TFTLCD_WrData(0x01);
	IERG3810_TFTLCD_WrData(0x3F);
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(y>>8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData(0x01);
	IERG3810_TFTLCD_WrData(0XDF);
	IERG3810_TFTLCD_WrReg(0X2C);
	IERG3810_TFTLCD_WrData(color);
}

void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 start_y, u16 length_x, u16 length_y) // Draw a rectangle
{
		u32 index = 0;
		IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(start_x>>8);
		IERG3810_TFTLCD_WrData(start_x & 0xFF);
		IERG3810_TFTLCD_WrData((length_x + start_x - 1)>>8);
		IERG3810_TFTLCD_WrData((length_x + start_x - 1) & 0xFF);
		IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(start_y>>8);
		IERG3810_TFTLCD_WrData(start_y & 0xFF);
		IERG3810_TFTLCD_WrData((length_y + start_y - 1)>>8);
		IERG3810_TFTLCD_WrData((length_y + start_y - 1) & 0xFF);
		IERG3810_TFTLCD_WrReg(0x2C);
		for(index = 0; index < length_x * length_y; index++)
		{
			IERG3810_TFTLCD_WrData(color);
		}
}

	void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color) // Show in standard character in ASCII code
	{
		u8 i,j;
		u8 index;
		u8 height =16, length=8;
		if(ascii<32||ascii>127) return;
		ascii -=32;
		IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x>>8);
		IERG3810_TFTLCD_WrData(x&0xFF);
		IERG3810_TFTLCD_WrData((length+x-1)>>8);
		IERG3810_TFTLCD_WrData((length+x-1) & 0xFF);
		IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y>>8);
		IERG3810_TFTLCD_WrData(y & 0xFF);
		IERG3810_TFTLCD_WrData((height+y-1)>>8);
		IERG3810_TFTLCD_WrData((height+y-1)&0xFF);
		IERG3810_TFTLCD_WrReg(0x2C);
		
		for (j=0; j<height/8;j++)
		{
			for(i=0; i<height/2;i++)
			{
				for(index=0;index<length; index++)
				{
					if((asc2_1608[ascii][index*2+1-j]>>i)&0x01) 
						IERG3810_TFTLCD_DrawDot(x + index, y+ 8*j + i, color);
				}
			}
		}
	}

void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 ascii,u16 color) // Draw the chinese char
{
    u8 i,j;
    u8 index;
    u8 height=16,length=16;
    //if(ascii<0||ascii>5) return;
    IERG3810_TFTLCD_WrReg(0x2A);
    IERG3810_TFTLCD_WrData(x >> 8);
    IERG3810_TFTLCD_WrData(x & 0xFF);
    IERG3810_TFTLCD_WrData((length + x -1) >>8);
    IERG3810_TFTLCD_WrData((length + x -1) & 0xFF);
    
    IERG3810_TFTLCD_WrReg(0x2B);
    IERG3810_TFTLCD_WrData(y >> 8);
    IERG3810_TFTLCD_WrData(y &0xFF);
    IERG3810_TFTLCD_WrData((height + y -1) >>8);
    IERG3810_TFTLCD_WrData((height + y -1) & 0xFF);
    IERG3810_TFTLCD_WrReg(0x2E);//LCD_WriteRAM_Prepare();
    
    for(j=0;j<height/8;j++)
    {
        for(i=0;i<height/2;i++)
        {
            for(index=0;index<length;index++)
            {
                if((chi_1616[ascii][index*2+1-j]>>i) & 0x01) 
									IERG3810_TFTLCD_DrawDot(x + index, y+ 8*j + i, color);
            }
        }
    }
}

void EXTI2_IRQHandler(void) // The handler of KEY2
{
	switch(speed)
	{
		case 1: speed = 3; break;
		case 3: speed = 1; break;
	}
		Delay(100000);
		EXTI->PR=1<<2;  //Clear this exception pending bit
}

void EXTIUP_IRQHandler(void)
{
    u8 i;
    for(i=0;i<10;i++)
    {
        GPIOB->BRR = 1 << 5;
        Delay(1000000);
        GPIOE->BSRR = 1 << 5;
        Delay(1000000);
    }
    EXTI->PR=1<<2;  //Clear this exception pending bit
}

void IERG3810_SYSTICK_Init10ms(void) // SYSTICK
{
    SysTick->CTRL= 0; // Clear Systick->CTRL setting
    SysTick->LOAD= 89999;
    
    SysTick->CTRL |= 0x00000003;
}

void EXTI15_10_IRQHandler(void) // The handler of PS2 Keyboard
{
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

void DrawAntiBirdPic( u16 x, u16 y, u8 mode ) // Draw the bird
{
	u16 color,xx = x,yy = y;
	int R,G,B;
	switch(mode) // Four status for the bird
	{
		case 'u': // upper
		{
			for( yy = y - 17; yy <= y; yy++ )
			{
				for( xx = x; xx < 26 + x; xx++ )
				{
					color = gImage_flappybird_bird_u[((y-yy)*26+(xx-x))*2+1];
					color <<= 8;
					color = color + gImage_flappybird_bird_u[((y-yy)*26+(xx-x))*2];
					R = color & 0xf800;
					R >>= 11;
					G = color & 0x07e0;
					G >>= 5;
					B = color & 0x001f;
					if((B - R) > 7)
						;
					else
						IERG3810_TFTLCD_DrawDot(xx, yy, color);
				}
			}
		}
		break;
		case 'm': // middle
		{
			for( yy = y - 17; yy <= y; yy++ )
			{
				for( xx = x; xx < 26 + x; xx++ )
				{
					color = gImage_flappybird_bird_m[((y-yy)*26+(xx-x))*2+1];
					color <<= 8;
					color = color + gImage_flappybird_bird_m[((y-yy)*26+(xx-x))*2];
					R = color & 0xf800;
					R >>= 11;
					G = color & 0x07e0;
					G >>= 5;
					B = color & 0x001f;
					if((B - R) > 7)
						;
					else
						IERG3810_TFTLCD_DrawDot(xx, yy, color);
				}
			}
		}
		break;
		case 'd': // down
		{
			for( yy = y - 17; yy <= y; yy++ )
			{
				for( xx = x; xx < 26 + x; xx++ )
				{
					color = gImage_flappybird_bird_d[((y-yy)*26+(xx-x))*2+1];
					color <<= 8;
					color = color + gImage_flappybird_bird_d[((y-yy)*26+(xx-x))*2];
					R = color & 0xf800;
					R >>= 11;
					G = color & 0x07e0;
					G >>= 5;
					B = color & 0x001f;
					if((B - R) > 7)
						;
					else
						IERG3810_TFTLCD_DrawDot(xx, yy, color);
				}
			}
		}
		break;
		case 'f': // fly
		{
			for( yy = y - 26; yy < y; yy++ )
			{
				for( xx = x; xx < 18 + x; xx++ )
				{
					color = gImage_flappybird_bird_m[((17-(xx-x))*26+(y-yy))*2+1];
					color <<= 8;
					color = color + gImage_flappybird_bird_m[((17-(xx-x))*26+(y-yy))*2];
					R = color & 0xf800;
					R >>= 11;
					G = color & 0x07e0;
					G >>= 5;
					B = color & 0x001f;
					if((B - R) > 7)
						;
					else
						IERG3810_TFTLCD_DrawDot(xx, yy, color);
				}
			}
		}
			break;
		default:
			break;
	}
}

void DrawTubeTop( u16 x, u16 y, u8 mode) // Draw the top of the tube
{
	
		u16 color,xx = x,yy = y;
		int R,G,B;
		if( mode == 0)
		{
			for( yy = y; yy < 21 + y; yy++ )
			{
				for( xx = x; xx < 39 + x; xx++ )
				{
					color = gImage_flappybird_column_bottom[((yy-y)*39+(xx-x))*2+1];
					color <<= 8;
					color = color + gImage_flappybird_column_bottom[((yy-y)*39+(xx-x))*2];
					R = color & 0xf800;
					R >>= 11;
					G = color & 0x07e0;
					G >>= 5;
					B = color & 0x001f;
					if((B - R) > 7)
						;
					else
						IERG3810_TFTLCD_DrawDot(xx, yy, color);
				}
			}
		}
		else if(mode == 1)
		{
			for( yy = y - 20; yy <= y; yy++ )
			{
				for( xx = x; xx < 39 + x; xx++ )
				{
					color = gImage_flappybird_column_bottom[((y-yy)*39+(xx-x))*2+1];
					color <<= 8;
					color = color + gImage_flappybird_column_bottom[((y-yy)*39+(xx-x))*2];
					R = color & 0xf800;
					R >>= 11;
					G = color & 0x07e0;
					G >>= 5;
					B = color & 0x001f;
					if((B - R) > 7)
						;
					else
						IERG3810_TFTLCD_DrawDot(xx, yy, color);
				}
			}
		}
}

void DrawTube( u16 x, u16 y) // Draw the lower part of the tube
{
	u16 color,xx = x,yy = y;
	int R,G,B;
	for( yy = y; yy < y + 7; yy++ )
	{
		for( xx = x; xx < 35 + x; xx++ )
		{
			color = gImage_flappybird_column_middle[((yy-y)*35+(xx-x))*2+1];
			color <<= 8;
			color = color + gImage_flappybird_column_middle[((yy-y)*35+(xx-x))*2];
			R = color & 0xf800;
			R >>= 11;
			G = color & 0x07e0;
			G >>= 5;
			B = color & 0x001f;
			if((B - R) > 7)
				;
			else
				IERG3810_TFTLCD_DrawDot(xx, yy, color);
		}
	}
}

void DrawLogo( u16 x, u16 y ) // Draw the flappy bird logo in the start page
{
		u16 color,xx = x,yy = y;
	
	for( yy = y - 35; yy <= y; yy++ )
	{
		for( xx = x; xx <= 136 + x; xx++ )
		{
			color = gImage_flappybird_Logo[((y-yy)*135+(xx-x))*2+1];
			color <<= 8;
			color = color + gImage_flappybird_Logo[((y-yy)*135+(xx-x))*2];
			IERG3810_TFTLCD_DrawDot(xx, yy, color);
		}
	}
	
}

void DrawNum_Small( u16 x, u16 y, u8 * num ) // Draw the number 0 - 9
{
	u16 color,xx = x,yy = y;
	int R,G,B;

	for( yy = y - 13; yy <= y; yy++ )
	{
		for( xx = x; xx < 10 + x; xx++ )
		{
			color = num[((y-yy)*9+(xx-x))*2+1];
			color <<= 8;
			color = color + num[((y-yy)*9+(xx-x))*2];
			R = color & 0xf800;
			R >>= 11;
			G = color & 0x07e0;
			G >>= 5;
			B = color & 0x001f;
			if((B - R) > 7);
			else
				IERG3810_TFTLCD_DrawDot(xx,yy,color);
		}
	}
}


void DrawNumber(u16 x, u16 y, u16 num)
{
			switch(num)
			{
				case 0:
					DrawNum_Small(x,y,(u8 *)gImage_0_small);
					break;
				case 1:
					DrawNum_Small(x,y,(u8 *)gImage_1_small);
					break;
				case 2:
					DrawNum_Small(x,y,(u8 *)gImage_2_small);
					break;
				case 3:
					DrawNum_Small(x,y,(u8 *)gImage_3_small);
					break;
				case 4:
					DrawNum_Small(x,y,(u8 *)gImage_4_small);
					break;
				case 5:
					DrawNum_Small(x,y,(u8 *)gImage_5_small);
					break;
				case 6:
					DrawNum_Small(x,y,(u8 *)gImage_6_small);
					break;
				case 7:
					DrawNum_Small(x,y,(u8 *)gImage_7_small);
					break;
				case 8:
					DrawNum_Small(x,y,(u8 *)gImage_8_small);
					break;
				case 9:
					DrawNum_Small(x,y,(u8 *)gImage_9_small);
					break;
			}
}


void DrawNum_Multi( u16 x, u16 y, u32 num, u8 mode ) // Draw number with multiple digits
{
	u16 bit_Num,i;
	u32 num_t;
	u8 oneBit_Num;
	num_t = num;
	
	bit_Num = 0;
	while(num_t)
	{
		num_t /= 10;
		bit_Num++;
	}
	if( 0 == num)
	{
		bit_Num = 1;
	}
	
	if( mode == 0)
	{
		num_t = num;
		for( i = 0; i < bit_Num; i++ )
		{
			oneBit_Num = num_t%10;
			DrawNumber(x + bit_Num*10-i*20,y,oneBit_Num);
			num_t /= 10;
		}	
	}
	else
	{
		num_t = num;
		for( i = 0; i < bit_Num; i++ )
		{
			oneBit_Num = num_t%10;
			DrawNumber(x-i*11,y,oneBit_Num);
			num_t /= 10;
		}
	}	
}

void AddWall() // Add a set of walls
{
	int index;
	for(index = 0; index <= 2; index ++)
	{
		if(wallPass[index] == 0)
		{
			wallX[index] = 240;
			wallY[index] = rand() % 100 + 39;
			wallPass[index] = 1;
			break;
		}	
	}
}

void GameDataInit() // Initailize the game data
{
	int i;
	next = 0;
	status = 1;
	score = 0;
	Number = 3;
	canStart = 0;
	canBeat2 = 0;
	canBeat3 = 0;
	touchXStatus = 0;
	OldXPosition = 10000;	
	ifBG = 0;
  b_x = 120;
	b_y = 160;
	speed = 1;
	
	GPIOB->BSRR = 1 << 5;
	GPIOE->BSRR = 1 << 5;
	
	for(i = 0; i <= 2; i ++ )
	{
		wallPass[i] = 0; // Disable all the walls
	}
}

void DrawBG() // Draw the background, in light blue
{
	IERG3810_TFTLCD_FillRectangle(LightBlue,0,0,240,320);
}

void DrawWall(int x, int y) // Draw the wall
{
	int yy;
	int count = 0;
	yy = y;
	while(yy >= 39) // Lower side
	{
		DrawTube(x, count*7);
		yy -= 7;
		count ++;
	}
	DrawTubeTop(x - 2,count*7,0);
	
	yy = y;
	count = 0;
	while( yy <= 161 ) // Upper side
	{
		DrawTube(x, 313 - count*7);
		yy += 7;
		count ++;
	}
	DrawTubeTop(x - 2, 320 - count*7,1);
}

void DrawBird(int x, int y) // Draw the flying bird
{
	DrawAntiBirdPic(x-13,y-9,mode[next]);
	next += 1;
	next %= 3;
}

void ifAddScore()
{
	int i;
	for( i = 0; i <=2; i ++ )
	{
		if(wallX[i] >= b_x - 20 && wallX[i] < b_x - 10 && wallPass[i] == 1)
		{
				score = score + 1;
				wallPass[i] = 2;
		}
	}
}
void ShowScore() // Draw the score
{
	DrawNum_Multi(112,220,score,1);
}

void GamePlay() // Show the current game view
{
	int index;
	DrawBG();
	DrawBird(b_x,b_y);
	for(index = 0; index <=2; index ++)
	{
		if(wallPass[index] == 1 || wallPass[index] == 2)
		{
			DrawWall(wallX[index],wallY[index]);
		}
	}
}

void ifFail() // Test if the bird hit the wall or be out of boundary
{
	int i;
	for(i = 0; i <= 2; i ++)
	{
		if(wallX[i] <= b_x + 14 && wallX[i] >= b_x -14 ) // If hit the wall
		{
			if( wallPass[i] == 1 || wallPass[i] == 2)
			{
				if(b_y <= wallY[i] + 9 + 7 || b_y >= wallY[i] + 120 + 15 )
				{
					status = 0;
					ifBG = 0;
					GPIOB->BRR = 1 << 5;
					GPIOE->BSRR = 1 << 5;
				}
			}
		}
		if(b_y >= 310 || b_y <= 10 || b_x <= 20 || b_x >= 220) // If out of boundary
		{
			status = 0;
			ifBG = 0;
			GPIOB->BRR = 1 << 5;
			GPIOE->BSRR = 1 << 5;
		}
	}
}

int main(void)
{	
	u8 i = 0;
	//u32 timeout_ps2 = 0;
	canStart = 0;
	
	char SID1[10]="1155092200"; // SID
  char SID2[10]="1155092202";
	char GRP[10]   ="GROUP: W10"; // Group Info
	
	char Info1[17] = "Enter: Start Game"; // Game meau
	char Info2[20] = "Button: Change Speed";
	char Info3[21] = "Player1: Touch to Fly";
	char Info4[20] = "Player2: 8 - Wall Up";
	char Info5[22] = "Player2: 2 - Wall Down";
	char Info6[25] = "Press Num Lock to Restart";
	
	IERG3810_clock_tree_init(); // Initailization of several hardware components
  IERG3810_LED_Init();
  IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_SYSTICK_Init10ms();
	IERG3810_TFTLCD_Init();
	IERG3810_key2_ExtiInit();
	IERG3810_keyUP_ExtiInit();
	IERG3810_PS2key_ExtiInit();
	IERG3810_TouchScreen_init();
	
	GameDataInit(); // Initailize the game data
	Delay(100000);

	while(1) // Get the PS2 keyborad input
	{
		int x;
		sheep++;
			
		if( canPrint == 1 )
		{	
			int count;
			for(count =0; count <=7; count ++)
			{
				ps2key |= store[count] << count;
			}
				
			for(x =0; x <= 7; x++)
			{
				store[x] = 0;
			}
				
			if(ps2key == 0x77 && status == 0) // If restart
			{
				GameDataInit();
				Delay(100000);
				GPIOB->BSRR = 1 << 5;
			}
			
			if(ps2key == 0x5A && canStart == 0) // If start
			{
				canStart = 1;
				canBeat2 = 1; // Start heart beat 2
				canBeat3 = 1; // Start heart beat 3
				GPIOB->BSRR = 1 << 5;
				GPIOE->BRR = 1 << 5;
			}
			
			if(ps2key == 0x75 && canStart == 1) // Wall up
			{
				int i;
				for( i = 0; i <=2; i ++)
					if(wallY[i] < 150)
					{
						wallY[i] += 15;
						ifFail();
					}
						
			}
			
			if(ps2key == 0x72 && canStart == 1) // Wall down
			{
				int i;
				for( i = 0; i <=2; i ++)
					if(wallY[i] > 20)
					{
						wallY[i] -= 15;
						ifFail();
					}
			}
			
			canPrint = 0;
			ps2key =0;
		}
		
		/*timeout--;
		if(timeout == 0)
		{
			timeout = 20000;
			for(x =0; x <= 7; x++)
			{
				store[x] = 0;
			}
			ps2count = 0;
		}*/
		
		if(task1HeartBeat >= 14 / speed) // Use SYSTick, 0.15 second a tick
    {
			if(canStart == 0) // Draw the start page
			{
				DrawBG();
				DrawAntiBirdPic(106,240,mode[next]);
				next = next + 1;
				next = next % 3;
				DrawLogo(52,300);
				
				for(i=20;i <120;i +=8) // Draw SID
				{
					IERG3810_TFTLCD_ShowChar(i, 20, SID1[i/8 - 4],White);
					IERG3810_TFTLCD_ShowChar(i, 40, SID2[i/8 - 4],White);
				}
				
				for(i=80;i <160;i +=8) // Draw Group
				{
					IERG3810_TFTLCD_ShowChar(i, 70, GRP[i/8 - 10],White);
				}
				
				for(i=52;i <52+8*17;i +=8) // Draw the meau
				{
					IERG3810_TFTLCD_ShowChar(i, 185, Info1[i/8 - 6],White);
				}
				for(i=40;i <200;i +=8)
				{
					IERG3810_TFTLCD_ShowChar(i, 165, Info2[i/8 - 5],White);
				}
				for(i=36;i <204;i +=8)
				{
					IERG3810_TFTLCD_ShowChar(i, 145, Info3[i/8 - 4],White);
				}
				for(i=40;i <200;i +=8)
				{
					IERG3810_TFTLCD_ShowChar(i, 125, Info4[i/8 - 5],White);
				}
				for(i=32;i <208;i +=8)
				{
					IERG3810_TFTLCD_ShowChar(i, 105, Info5[i/8 - 4],White);
				}
				
				for(i = 20; i <= 220; i ++)
				{
					IERG3810_TFTLCD_DrawDot(i,203,White);
					IERG3810_TFTLCD_DrawDot(i,100,White);
				}
				for(i = 100; i <= 203; i ++)
				{
					IERG3810_TFTLCD_DrawDot(20,i,White);
					IERG3810_TFTLCD_DrawDot(220,i,White);
				}
				IERG3810_TFTLCD_ShowChinChar(150, 20, 0,White); // Draw group member's name
				IERG3810_TFTLCD_ShowChinChar(170, 20, 1,White);
				IERG3810_TFTLCD_ShowChinChar(190, 20, 2,White);
				IERG3810_TFTLCD_ShowChinChar(150, 40, 3,White);
				IERG3810_TFTLCD_ShowChinChar(170, 40, 4,White);
				IERG3810_TFTLCD_ShowChinChar(190, 40, 5,White);
				task1HeartBeat = 0;
			}
		
			if(canStart == 1) // Draw the game page
			{
				if(status == 1)
				{
					GamePlay(); 
					if(canBeat3 == 0)
						DrawNum_Multi(112,250,score,1);
					/*
					TsYposition = TouchScreenReadData(1); //read data from the tochscreen, 
					TsXposition = TouchScreenReadData(5); //read data from the tochscreen, 
					
					if( TsXposition > 10000  && TsYposition > 10000 && touchXStatus == 0) // If the touch screen is pressed
					{
						b_y += 30;
						ifFail();
					}*/
					
					for(i = 0; i <= 2; i ++) // Test whether we need to add one to the current score
					{
						wallX[i] = wallX[i] - 5;
						b_y = b_y - 2; 
						if(wallX[i] <= 0)
						{
							wallPass[i] = 0;
						}
					}
					ifAddScore();
					ifFail();
				}
				if(status == 0) // Draw the game over page
				{
					DrawBG();
					DrawAntiBirdPic(106,150,mode[next]);
					ShowScore();
					for(i=20;i <220;i +=8)
					{
						IERG3810_TFTLCD_ShowChar(i, 50, Info6[i/8 - 2],White);
					}
					next = next + 1;
					next = next % 3;
				}
				task1HeartBeat = 0;
    }
	}
		if(task2HeartBeat >= 599 / speed ) // Use a slower tick to add wall
		{
			if(canStart == 1 && status == 1)
			{
				AddWall();
			}
			task2HeartBeat = 0;
		}
		
		if(task3HeartBeat >= 100) // Draw the count down number when game 
		{
			if(Number != 0)
				DrawNum_Multi(112,280,Number,1);
			task3HeartBeat = 0;
			Number --;
			if(Number == -1)
					canBeat3 = 0;
		}
		
		if(task4HeartBeat >= 5)
		{
			if(status == 1)
			{
				TsXposition = TouchScreenReadData(5);			 //read data from the tochscreen
				if(TsXposition > 10000)
				{
					if(TsXposition - OldXPosition > 50 && touchXStatus == 1)
					{
						b_x += 10;
						b_y += 10 + speed / 2 * 5;
						ifAddScore();
						ifFail();	
						OldXPosition = TsXposition;		
					}
					if(TsXposition - OldXPosition < -50 && touchXStatus == 1)
					{
						b_x -= 10;
						b_y += 10 + speed / 2 * 5;
						ifFail();	
						OldXPosition = TsXposition;
					}
					if(OldXPosition == 10000)
					{						
						if(TsXposition - OldXPosition > 30 || TsXposition - OldXPosition < -30)
							touchXStatus = 1;
					}
				}
				if(TsXposition <= 10000 && touchXStatus == 1)
				{
					touchXStatus = 0;
					OldXPosition = 10000;
				}
				task4HeartBeat = 0;
			}
		}
	}
}
