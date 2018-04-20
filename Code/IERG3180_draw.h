#ifndef _IERG3180_DRAW.H
#define _IERG3180_DRAW.H
#include "IERG3810_LCD.h"
#include "stm32f10x.h"

void draw_start(void);
void draw_play_bg(void);
void draw_bird(int b_y);
void draw_wall(int, int, int);
void draw_play_score(int);
void draw_game_over(int);

#endif