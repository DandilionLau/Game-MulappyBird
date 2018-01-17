#include "stm32f10x.h"
#include "Board.h"


void U16toStr(char *st , u16 value)
{
	st[0] = value/10000+48;
	st[1] = value/1000%10+48;
	st[2] = value/100%10+48;
	st[3] = value/10%10+48;	
	st[4] = value%10+48;		
}
