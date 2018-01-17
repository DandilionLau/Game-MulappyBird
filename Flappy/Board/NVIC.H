#ifndef __NVIC_H
#define __NVIC_H 	   

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup)
{
    u32 temp,temp1;
    temp1=prigroup & 0x00000007;
    temp1<<=8;
    temp = SCB->AIRCR;
    temp &=0x0000F8FF;
    temp|=0x05FA0000;
    temp|=temp1;
    SCB->AIRCR=temp;
}

#endif
