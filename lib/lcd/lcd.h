#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

#include "stm32f4xx_hal.h"

void LCD_Init();
void LCD_Clear();
void LCD_Delay(volatile int time_us);
void LCD_WriteChar(char ch);
void LCD_WriteString(char *s, int n);
void LCD_OverWriteString(char *s, int n);

#endif