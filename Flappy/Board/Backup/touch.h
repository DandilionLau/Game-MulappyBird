 /* 头文件 touch.h */

#ifndef TOUCH_H
#define TOUCH_H

/* 添加驱动文件 */
//#include "Diver.h"

typedef unsigned short int uint16_t ;

typedef unsigned char uint8_t;

typedef short int int16_t;

/* 触摸屏必要数据结构体 */
typedef struct
{
    /* 得到触摸屏X通道AD数据的方法 */
    uint16_t (* const Get_XChannel)(void);

    /* 得到触摸屏Y通道AD数据的方法 */
    uint16_t (* const Get_YChannel)(void);

    /* 延时函数 */
    void (*Delay_ms)(uint16_t);

    /* 查看触摸屏是否被按下  返回0表示未按下 否则被按下 */
    uint8_t (*Get_StatusFlag)(void);

    void (*Clear_PendingBit)(void);

    /* 在液晶屏上显示十字号函数 */
    void (*DispH)(uint16_t x ,uint16_t y);

    /* 液晶屏的X=触摸屏的X*TP_Xk+TP_Xoffset Y坐标也同理*/
    float Xk;

    float Yk;

    float Xoffset;

    float Yoffset;

}Tuch_TypeDef;

/* 坐标结构体 */
typedef struct
{
    uint16_t TP_X;

    uint16_t TP_Y;

}Touch_CoorDinate;


#define Calibration_x1  50
#define Calibration_y1  50

#define Calibration_x2  50
#define Calibration_y2  190

#define Calibration_x3  270
#define Calibration_y3  190

#define Calibration_x4  270
#define Calibration_y4  50


/* 得到液晶屏的坐标 返回值0 坐标靠谱 否则不靠谱 */
uint8_t Touch_GetCoordinate(Touch_CoorDinate *TP_COOR);

/* 触摸屏校准 返回0 校准成功  否则失败 */
uint8_t TP_Calibration(void);



#endif // TOUCH_H_INCLUDED