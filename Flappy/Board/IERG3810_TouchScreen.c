#include "stm32f10x.h"
#include "Board.h"
#include "IERG3810_TouchScreen.h"

/*Touch Screen, 
Connected to STM32 GPIOs, primitive SPI function cannot be applied
Use GPIOs to similate SPI fuction.
T_MISO, data from touch screen to CPU, PF8, GPIO-Input, pull-high
T_MOSI, data from CPU to touch screen, PF9, GPIO-Output PP-mode
T_SCK,  SPI clock from CPU,  PB1, GPIO-OUTPUT, PP
T_PEN, 	Interrupt from touch screen to  CPU, PF10, EXTI-10
T_CS, 	chip select from CPU to touch screen, PB12, GPIO-output PP-mode 


*/


void IERG3810_TouchScreen_init(void)
{
		RCC->APB2ENR |=1 <<3;  //enable Port-B 
		GPIOB->CRL &= 0xFFFFFF0F; //PB1
		GPIOB->CRL |= 0x00000010; //PB1 = GPIO-output, 10MHz, Push-pull
		GPIOB->BRR = 1 << 1;    //PB1  = default low	
		GPIOB->CRH &= 0xFFF0FFFF; //PB12	
		GPIOB->CRH |= 0x00010000; //PB12 = GPIO-output, 10MHz, Push-pull
		GPIOB->BSRR = 1 << 12;    //PB12  = default high	 	
		RCC->APB2ENR |=1 <<7;  //enable Port-F 
		GPIOF->CRH &= 0xFFFFF000; //PF8, PF9, PF10
		GPIOF->CRH |= 0x00000008; //PF8 = GPIO-input, Pull-up 		
		GPIOF->CRH |= 0x00000010; //PF9 =  GPIO-output, 10MHz, Push-pull 	
		GPIOF->CRH |= 0x00000800; //PF10 =  GPIO-input,Pull-up
		GPIOF->BSRR = 1 << 8;    //PF8  = default high
		GPIOF->BSRR = 1 << 9;    //PF9  = default high
		GPIOF->BSRR = 1 << 10;   //PF10 = default high
}

u16 TouchScreenReadData(u8 address)   
//read data from the tochscreen, input:1=Y, 3=Z1, 4=Z2, 5=X
{
	u16 value=0;
	TouchScreenCS(0);
	TouchScreenMOSI(1);      //start
	TouchScreenSckPulse();	 //clock pulse for start
	if ((GPIOF->IDR & 0x01<<10) == 0) value = 10000;
	if (address /4%2 ==1) TouchScreenMOSI(1); //A2    
	else TouchScreenMOSI(0);
	TouchScreenSckPulse();  //clock pulse for A2
	if (address /2%2 ==1) TouchScreenMOSI(1); //A1    
	else TouchScreenMOSI(0);
	TouchScreenSckPulse();  //clock pulse for A1
	if (address %2 ==1) TouchScreenMOSI(1); //A0    
	else TouchScreenMOSI(0);  //A0
	TouchScreenSckPulse();	//clock pulse for A0
	TouchScreenMOSI(1);				//mode-1, 8-bit
	TouchScreenSckPulse();	//clock pulse for MODE
	TouchScreenMOSI(1);				//SER
	TouchScreenSckPulse();	//clock pulse for SER
	TouchScreenMOSI(0);   //PD1
	TouchScreenSckPulse();	//clock pulse for PD1	
	TouchScreenMOSI(0);				//PD0
	TouchScreenSckPulse();	//clock pulse for PD0
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 2048;
	TouchScreenSckPulse();	//clock pulse for D11
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 1024;		
	TouchScreenSckPulse();	//clock pulse for D10
	if ((GPIOF->IDR & 0x01<<8) >0) value += 512;	
	TouchScreenSckPulse();	//clock pulse for D9		
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 256;
	TouchScreenSckPulse();	//clock pulse for D8
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 128;	
	TouchScreenSckPulse();	//clock pulse for D7
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 64;	
	TouchScreenSckPulse();	//clock pulse for D6	
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 32;	
	TouchScreenSckPulse();	//clock pulse for D5	
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 16;	
	TouchScreenSckPulse();	//clock pulse for D4
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 8;	
	TouchScreenSckPulse();	//clock pulse for D3	
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 4;	
  TouchScreenSckPulse();	//clock pulse for D2
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 2;	
	TouchScreenSckPulse();	//clock pulse for D1
	if ((GPIOF->IDR & 0x01<<8) > 0) value += 1;	
	TouchScreenSckPulse();	//clock pulse for D0
	TouchScreenSckPulse();	//clock pulse for idle	
  TouchScreenSckPulse();	//clock pulse for idle
	TouchScreenSckPulse();	//clock pulse for idle
	TouchScreenSckPulse();	//clock pulse for idle	
	TouchScreenCS(1);
	return value;
}



void TouchScreenSckPulse(void)
{	
	GPIOB->BSRR = 1 << 1;  //PB1 = high
	//GPIOB->BSRR = 1 << 1;  //PB1 = high	
	//Delay(1);
	GPIOB->BRR = 1 << 1;   //PB1 = low
	//GPIOB->BRR = 1 << 1;   //PB1 = low
	//Delay(1);
}

void TouchScreenCS(u8 i)
{
	if (i==1) GPIOB->BSRR = 1 << 12;   //PB12 = high
	else GPIOB->BRR = 1 << 12;				//PB12 = low	
}

void TouchScreenMOSI(u8 i)
{
	if (i==1) GPIOF->BSRR = 1 << 9;  //BF9 = high
	else	GPIOF->BRR = 1 << 9;  //BF9 = low
}
