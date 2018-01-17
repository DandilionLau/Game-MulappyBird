#include "IERG3810_LCD.h"
#include "stm32f10x.h"



void draw_start(void)
{
// the name, press A to begin
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

}

void draw_game_over(int score)
{
// game over
//show the total score
//press A to restart
}
