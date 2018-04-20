#include "IERG3810_LCD.h"
#include "stm32f10x.h"

int temp;
int temp1;
int i;
char[23] s="Welcome to flappy bird.";
char[21] s1="Press keyUp to start.";
char[10] s2="GAME OVER!";
char[26] s3="Press keyUp to go to host.";
char[20] s4="The total score is: "
char[10] SID1="1155092200";
char[10] SID2="1155092202";
void draw_start(void)
{
   IERG3810_TFTLCD_FillRectangle(0xFFFF, 0, 0, 240, 320);
  
   for(i=0;i<23;i++)
   {
   IERG3810_TFTLCD_ShowChar(40+i*8, 180, s[i], 0x0000, 0xFFFF);
   }
   for(i=0;i<20;i++)
   {
   IERG3810_TFTLCD_ShowChar(40+i*8, 130, s1[i], 0x0000, 0xFFFF);
   }
  /*
  for(i=0;i<10;i++)
  {
  IERG3810_TFTLCD_ShowChar(40+i*8, 100, SID1[i], 0x0000, 0xFFFF);
  IERG3810_TFTLCD_ShowChar(40+i*8, 70, SID2[i], 0x0000, 0xFFFF);
  }
  IERG3810_TFTLCD_ShowChinChar(140, 100, 0,0x0000);
  IERG3810_TFTLCD_ShowChinChar(156, 100, 1,0x0000);
  IERG3810_TFTLCD_ShowChinChar(172, 100, 2,0x0000);
  IERG3810_TFTLCD_ShowChinChar(140, 70, 3,0x0000);
  IERG3810_TFTLCD_ShowChinChar(156, 70, 4,0x0000);
  IERG3810_TFTLCD_ShowChinChar(172, 70, 5,0x0000);
  */
}

void draw_play_bg(void)
{
IERG3810_TFTLCD_FillRectangle(0xFFFF, 0, 0, 240, 320);
}

void draw_bird(int x, int y)
{
 IERG3810_TFTLCD_FillRectangle(0xF800, x, y, 5, 5)
}

void draw_wall(int x, int upper_y, int lower_y,int wall_length)
{
  IERG3810_TFTLCD_FillRectangle(0x07E0, x, upper_y, wall_length,  320-upper_y);
  IERG3810_TFTLCD_FillRectangle(0x07E0, x, 0, wall_length,  lower_y);
}

void draw_play_score(int score)
{
   i=0;
   temp=score;
   while(temp!=0){
    temp1=temp%temp;
    temp=temp/10;
   	IERG3810_TFTLCD_SevenSegment(0xFFE0,40-8*i,300, temp1);
   	i++;
   	}
}

void draw_game_over(int score)
{
    IERG3810_TFTLCD_FillRectangle(0xFFFF, 0, 0, 240, 320);
    for(i=0;i<20;i++)
   	{
  	 IERG3810_TFTLCD_ShowChar(40+i*8, 180, s2[i], 0x0000, 0xFFFF);
   	}
   	for(i=0;i<26;i++)
   	{
  	 IERG3810_TFTLCD_ShowChar(40+i*8, 100, s3[i], 0x0000, 0xFFFF);
   	}
   	for(i=0;i<20;i++)
   	{
  	 IERG3810_TFTLCD_ShowChar(40+i*8, 160, s4[i], 0x0000, 0xFFFF);
   	}
   i=0;
   temp=score;
   while(temp!=0){
    temp1=temp%temp;
    temp=temp/10;
   	IERG3810_TFTLCD_SevenSegment(0xFFE0,160-8*i,140, temp1);
   	i++;
   	}
}
