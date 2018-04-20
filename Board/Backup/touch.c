/* 源文件 touch.c */

/*
    名称:电阻触摸屏的校准程序V1.0版
    作者:灼灼其华

    移植的时候只要给 Touch_Structure 这个变量赋初值 就可以了

    Get_XChannel        得到X通道的数据的方法

    Get_YChannel        得到Y通道数据的方法

    Delay_ms            毫秒级延时函数

    Get_StatusFlag      检测触摸屏被按下的方法 0 没有按下  1 已结按下了

    Clear_PendingBit    清除触摸屏按下标志的方法

    DispH               在液晶屏上显示十字号的方法

    移植之前要先实现这几个简单的函数/方法
*/

#include "touch.h"

#define N   10
/* 保存点击一次触摸屏 采集的数据 */
static uint16_t TP_Coordinate[2][10];

/* 给Touch_Structure 变量赋初值 */
static Tuch_TypeDef   Touch_Structure;


#define R   50
/* 触摸屏数据滤波 */

int ifTouch()
{
	return Touch_Structure.Get_StatusFlag();
}
uint16_t TP_Data_Processing(uint16_t *pData,uint8_t Length)
{
    uint8_t i,j,Cnt=0,Max=0;

    uint16_t Number,cc;

    for(i=0;i<Length;i++)
    {
        for(j=0;j<Length;j++)
        {
            if(i==j)
                continue;

            if(pData[i]>pData[j])
                cc=pData[i]-pData[j];
            else
                cc=pData[j]-pData[i];

            if(cc<R)
            {
                Cnt++;
            }
        }

        if(Cnt>Max)
        {
            Max=Cnt;
            Number=pData[i];
        }
        Cnt=0;
    }

    return Number;
}

/* 求二阶行列式的值 */
int16_t Get_Determinant_2(int16_t a11,int16_t a12,int16_t a21,int16_t a22)
{
    return (a11*a22-a21*a12);
}

/* 解二元一次方程 前两列保存的是系数 */
uint8_t Solve_Equations_2(int16_t(*ptr)[3],float *k,float *off)
{
    int16_t D,D1,D2;

    D=Get_Determinant_2(ptr[0][0],ptr[0][1],ptr[1][0],ptr[1][1]);

    if(D==0)
        return 1;

    D1=Get_Determinant_2(ptr[0][2],ptr[0][1],ptr[1][2],ptr[1][1]);

    D2=Get_Determinant_2(ptr[0][0],ptr[0][2],ptr[1][0],ptr[1][2]);

    *k=D1/(float)D;

    *off=D2/(float)D;

    return 0;
}

/* 求绝对值 */
uint16_t Get_ABS(uint16_t x,uint16_t y)
{
    if(x>y)
        return x-y;
    else
        return y-x;
}


// 允许的误差 */
#define CCMAX   50

/* 得到触摸屏的坐标 并且滤波 返回0 表示坐标靠谱   否则是不靠谱的 */
uint8_t TP_GetCoorDinate(Touch_CoorDinate *TP_COOR)
{
    uint8_t bit;

    uint16_t TP_XCoor_1,TP_YCoor_1;
    uint16_t TP_XCoor_2,TP_YCoor_2;
    /* 第一次采集数据 */
    for(bit=0;bit<N;bit++)
    {
        /* 采集10次 X 数据 */
        TP_Coordinate[0][bit]=Touch_Structure.Get_XChannel();

        /* 采集10次 Y 数据 */
        TP_Coordinate[1][bit]=Touch_Structure.Get_YChannel();
    }

    TP_XCoor_1=TP_Data_Processing(TP_Coordinate[0],N);

    TP_YCoor_1=TP_Data_Processing(TP_Coordinate[1],N);

    /* 第二次采集数据 */
    for(bit=0;bit<N;bit++)
    {
        /* 采集10次 X 数据 */
        TP_Coordinate[0][bit]=Touch_Structure.Get_XChannel();

        /* 采集10次 Y 数据 */
        TP_Coordinate[1][bit]=Touch_Structure.Get_YChannel();
    }

    TP_XCoor_2=TP_Data_Processing(TP_Coordinate[0],N);

    TP_YCoor_2=TP_Data_Processing(TP_Coordinate[1],N);

    /* 两次采集的数据之差不能超过CCMAX 否则认为数据不靠谱 */

    if( Get_ABS(TP_XCoor_1 , TP_XCoor_2) >CCMAX )
        return 1;

    if( Get_ABS(TP_YCoor_1 , TP_YCoor_2) >CCMAX )
        return 1;

    /* 取两次采集数据的平均值 */
    TP_COOR->TP_X=(TP_XCoor_1>>1)+(TP_XCoor_2>>1);

    TP_COOR->TP_Y=(TP_YCoor_1>>1)+(TP_YCoor_2>>1);
}

/* 校准误差 */
#define CCMA        100

/* 触摸屏校准 */
uint8_t TP_Calibration(void)
{
        Touch_CoorDinate TP_COOR[4];

    int16_t arr[2][3];

    uint16_t CC,CC1,CC2;

    /* 校准第一个点 */
    Touch_Structure.Delay_ms(500);

    Touch_Structure.DispH(Calibration_x1,Calibration_y1);

    Touch_Structure.Clear_PendingBit();

    while(1)
    {
        if(Touch_Structure.Get_StatusFlag())
        {
            Touch_Structure.Clear_PendingBit();

            if(TP_GetCoorDinate(TP_COOR))
            {
                /* 显示重新点击 */
            }
            else
                break;
        }
    }


    /* 校准第二个点 */
    Touch_Structure.Delay_ms(500);

    Touch_Structure.DispH(Calibration_x2,Calibration_y2);

    Touch_Structure.Clear_PendingBit();

    while(1)
    {
        if(Touch_Structure.Get_StatusFlag())
        {
            Touch_Structure.Clear_PendingBit();

            if(TP_GetCoorDinate(TP_COOR+1))
            {
                /* 显示重新点击 */
            }
            else
                break;
        }
    }


    /* 校准第三个点 */
    Touch_Structure.Delay_ms(500);

    Touch_Structure.DispH(Calibration_x3,Calibration_y3);

    Touch_Structure.Clear_PendingBit();

    while(1)
    {
        if(Touch_Structure.Get_StatusFlag())
        {
            Touch_Structure.Clear_PendingBit();

            if(TP_GetCoorDinate(TP_COOR+2))
            {
                /* 显示重新点击 */
            }
            else
                break;
        }
    }


     /* 校准第四个点 */
    Touch_Structure.Delay_ms(500);

    Touch_Structure.DispH(Calibration_x4,Calibration_y4);

    Touch_Structure.Clear_PendingBit();

    while(1)
    {
        if(Touch_Structure.Get_StatusFlag())
        {
            Touch_Structure.Clear_PendingBit();

            if(TP_GetCoorDinate(TP_COOR+3))
            {
                /* 显示重新点击 */
            }
            else
                break;
        }
    }

    /* 检查数据是否准确 */

    CC1=Get_ABS(TP_COOR[0].TP_X , TP_COOR[3].TP_X);

    CC2=Get_ABS(TP_COOR[2].TP_X , TP_COOR[1].TP_X);

    CC=Get_ABS(CC1,CC2);

    if(CC>CCMA)
        return 1;

    CC1=Get_ABS(TP_COOR[0].TP_Y , TP_COOR[1].TP_Y);

    CC2=Get_ABS(TP_COOR[2].TP_Y , TP_COOR[3].TP_Y);

    CC=Get_ABS(CC1,CC2);

    if(CC>CCMA)
        return 1;

    /* 解方程求 TP_Xk TP_Yk TP_Xoffset TP_Yoffset */
    /* 触摸屏坐标*K+offset =液晶屏坐标 */
    arr[0][0]=TP_COOR[0].TP_X; arr[0][1]=1; arr[0][2]=Calibration_x1;

    arr[1][0]=TP_COOR[2].TP_X; arr[1][1]=1; arr[1][2]=Calibration_x3;

    if(Solve_Equations_2(arr , &Touch_Structure.Xk , &Touch_Structure.Xoffset))
    {
        /* 显示方程无解 数据错误 */

        return 1;
    }

    arr[0][0]=TP_COOR[1].TP_Y; arr[0][1]=1; arr[0][2]=Calibration_y2;

    arr[1][0]=TP_COOR[3].TP_Y; arr[1][1]=1; arr[1][2]=Calibration_y4;

    if(Solve_Equations_2(arr , &Touch_Structure.Yk , &Touch_Structure.Yoffset))
    {
        /* 显示方程无解 数据错误 */

        return 1;
    }

        return 0;
}

/* 得到液晶屏的坐标 返回值0 坐标靠谱 否则不靠谱 */
uint8_t Touch_GetCoordinate(Touch_CoorDinate *TP_COOR)
{
    if(TP_GetCoorDinate(TP_COOR))
        return 1;

    /* 数据一般不会得到极值 */
    /*
    if(TP_COOR->TP_X==0||TP_COOR->TP_X==4095 ||TP_COOR->TP_Y==0 ||TP_COOR->TP_Y==4095)
        return 1;
    */

    TP_COOR->TP_X=TP_COOR->TP_X*Touch_Structure.Xk+Touch_Structure.Xoffset;

    TP_COOR->TP_Y=TP_COOR->TP_Y*Touch_Structure.Yk+Touch_Structure.Yoffset;

    return 0;
}