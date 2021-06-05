#ifndef __IERG3810_TFTLCD_H
#define __IERG3810_TFTLCD_H
#include "stm32f10x.h"

// put procedure header here


void IERG3810_TFTLCD_Init(void);
void IERG3810_TFTLCD_SetParameter(void);
void IERG3810_TFTLCD_WrReg(u16 regval);
void IERG3810_TFTLCD_WrData(u16 data);
void IERG3810_TFTLCD_DrawDot(u16 x, u16 y,u16 color);
void IERG3810_TFTLCD_FillRectangle(u16 color,u16 start_x, u16 length_x, u16 start_y, u16 length_y);
void IERG3810_TFTLCD_ShowChar(u16 x,u16 y,u8 ascii, u16 color, u16 bgcolor);
void IERG3810_TFTLCD_SevenSegment (u16 color, u16 start_x, u16 start_y, u8 digit);
void IERG3810_TFTLCD_ShowChinChar(u16 x,u16 y,u8 code, u16 color, u16 bgcolor);
void DrawLine_3_1 ();
void DrawRectangle_3_2 ();
void CountDigits_3_3 ();
void ShowCUSID_3_4 ();
void ShowChineseName_3_5 ();
void CountDown();

#endif
