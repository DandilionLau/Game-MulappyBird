#ifndef __DRAW_H
#define __DRAW_H
#include "LCD.h"
#include "stm32f10x.h"
void draw_start(void);
void draw_play_bg(void);
void draw_bird(int x, int y);
void draw_wall(int x, int upper_y, int lower_y, int wall_length);
void draw_play_score(int);
void draw_game_over(int);
#endif