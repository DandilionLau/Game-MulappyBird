#include "stm32f10x.h"
#include "FONT.H"
#include "CFONT.H"

#define LCD_LIGHT_ON GPIOB->ODR=1<<0
#define LCD_RD GPIOD->ODR=1<<4

typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
}	LCD_TypeDef;


#define LCD_BASE ((u32)(0x6C000000 |0x000007FE))
#define LCD	((LCD_TypeDef *) LCD_BASE)

void IERG3810_TFTLCD_WrReg(u16 regval)
{
	LCD->LCD_REG=regval;
}

void IERG3810_TFTLCD_WrData(u16 data)
{
	LCD->LCD_RAM=data;
}

void IERG3810_TFTLCD_SetParameter(void)
{
	IERG3810_TFTLCD_WrReg(0X01);
	IERG3810_TFTLCD_WrReg(0X11);
	
	IERG3810_TFTLCD_WrReg(0X3A);
	IERG3810_TFTLCD_WrData(0X55);
	
	IERG3810_TFTLCD_WrReg(0X29);
	IERG3810_TFTLCD_WrReg(0X36);
	IERG3810_TFTLCD_WrData(0XCA);
}


void IERG3810_TFTLCD_Init(void)
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
}


void IERG3810_TFTLCD_DrawDot(u16 x,u16 y, u16 color){
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

void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 start_y, u16 length_x, u16 length_y)
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

void draw_segment_a(u16 color,u16 start_x,u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x+ 10, start_y+130, 55, 10);
}

void draw_segment_d(u16 color,u16 start_x,u16 start_y)
{
    IERG3810_TFTLCD_FillRectangle(color, start_x+ 10, start_y+ 0, 55, 10);
}

void draw_segment_g(u16 color,u16 start_x,u16 start_y)
{
    IERG3810_TFTLCD_FillRectangle(color, start_x+ 10, start_y+ 65, 55 ,10);
}

void draw_segment_b(u16 color,u16 start_x,u16 start_y)
{
    IERG3810_TFTLCD_FillRectangle(color, start_x+ 65, start_y+ 75, 10, 55);
}

void draw_segment_c(u16 color,u16 start_x,u16 start_y)
{
    IERG3810_TFTLCD_FillRectangle(color, start_x+ 65, start_y+ 10, 10, 55);
}

void draw_segment_f(u16 color,u16 start_x,u16 start_y)
{
    IERG3810_TFTLCD_FillRectangle(color, start_x+ 0, start_y+ 75, 10,55);
}

void draw_segment_e(u16 color,u16 start_x,u16 start_y)
{
    IERG3810_TFTLCD_FillRectangle(color, start_x+ 0, start_y+ 10, 10, 55);
}

void IERG3810_TFTLCD_SevenSegment(u16 color,u16 start_x,u16 start_y, u8 digit)
{
    switch (digit) {
        case 0x00:
        {
             draw_segment_a(color,start_x,start_y);
             draw_segment_b(color,start_x,start_y);
             draw_segment_c(color,start_x,start_y);
             draw_segment_d(color,start_x,start_y);
             draw_segment_e(color,start_x,start_y);
             draw_segment_f(color,start_x,start_y);
					break;
        }
        case 0x01:
        {
             draw_segment_b(color,start_x,start_y);
             draw_segment_c(color,start_x,start_y);
					break;
        }
        case 0x02:
        {
             draw_segment_a(color,start_x,start_y);
             draw_segment_b(color,start_x,start_y);
             draw_segment_g(color,start_x,start_y);
             draw_segment_e(color,start_x,start_y);
             draw_segment_d(color,start_x,start_y);
					break;
        }
        case 0x03:
        {
             draw_segment_a(color,start_x,start_y);
             draw_segment_b(color,start_x,start_y);
             draw_segment_g(color,start_x,start_y);
             draw_segment_c(color,start_x,start_y);
             draw_segment_d(color,start_x,start_y);
					break;
        }
        case 0x04:
        {
             draw_segment_f(color,start_x,start_y);
             draw_segment_b(color,start_x,start_y);
             draw_segment_g(color,start_x,start_y);
             draw_segment_c(color,start_x,start_y);
					break;
        }
        case 0x05:
        {
             draw_segment_a(color,start_x,start_y);
             draw_segment_f(color,start_x,start_y);
             draw_segment_g(color,start_x,start_y);
             draw_segment_c(color,start_x,start_y);
             draw_segment_d(color,start_x,start_y);
					break;
        }
        case 0x06:
        {
             draw_segment_a(color,start_x,start_y);
             draw_segment_f(color,start_x,start_y);
             draw_segment_g(color,start_x,start_y);
             draw_segment_c(color,start_x,start_y);
             draw_segment_d(color,start_x,start_y);
             draw_segment_e(color,start_x,start_y);
					break;
        }
        case 0x07:
        {
             draw_segment_a(color,start_x,start_y);
             draw_segment_b(color,start_x,start_y);
             draw_segment_c(color,start_x,start_y);
					break;
        }
        case 0x08:
        {
            draw_segment_a(color,start_x,start_y);
            draw_segment_b(color,start_x,start_y);
            draw_segment_c(color,start_x,start_y);
            draw_segment_d(color,start_x,start_y);
            draw_segment_e(color,start_x,start_y);
            draw_segment_f(color,start_x,start_y);
            draw_segment_g(color,start_x,start_y);
					break;
        }
        case 0x09:
        {
            draw_segment_a(color,start_x,start_y);
            draw_segment_b(color,start_x,start_y);
            draw_segment_c(color,start_x,start_y);
            draw_segment_d(color,start_x,start_y);
            draw_segment_f(color,start_x,start_y);
            draw_segment_g(color,start_x,start_y);
					break;
        }
    }
	} 

	void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bgcolor)
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
	

void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 ascii,u16 color)
{
    u8 i,j;
    u8 index;
    u8 height=16,length=16;
    if(ascii<0||ascii>5) return;
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


/*
void IERG3810_TFTLCD_ShowChinCharSpecial(u16 x, u16 y, u8 ascii,u16 color)
{
    u8 i,j;
    u8 index;
    u8 height=16,length=16;
    if(ascii<0||ascii>5) return;
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
									IERG3810_TFTLCD_DrawDot(x+i*2,y+j*8+index,color);
            }
        }
    }
}*/