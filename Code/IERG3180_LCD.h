#ifndef _IERG3180_LCD.H
#define _IERG3180_LCD.H

#include "stm32f10x.h"
#include "FONT.H"
#include "CFONT.H"

typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
}	LCD_TypeDef;

#define LCD_BASE ((u32)(0x6C000000 |0x000007FE))
#define LCD	((LCD_TypeDef *) LCD_BASE)

void IERG3810_TFTLCD_WrReg(u16);
void IERG3810_TFTLCD_WrData(u16);
u16 LCD_RD_DATA(void);
u16 IERG3810_TFT_ReadReg(u16);
void IERG3810_TFTLCD_SetParameter(void);
void IERG3810_TFTLCD_Init(void);
void IERG3810_TFTLCD_DrawDot(u16,u16,u16);
void IERG3810_TFTLCD_FillRectangle(u16, u16, u16,u16, u16);
void IERG3810_TFTLCD_SevenSegment(u16,u16,u16, u8);
void IERG3810_TFTLCD_ShowChar(u16, u16, u8, u16, u16);
void IERG3810_TFTLCD_ShowChinChar(u16, u16, u8,u16);

#endif