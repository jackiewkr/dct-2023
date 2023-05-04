#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

#include "stm32f4xx_hal.h"

/*
 * LCD Connections
 * Vss -> Ground
 * Vdd -> 5V
 * Vd -> Output of potentiometer between 5V and Ground
 * LED Anode -> 5V
 * LED Cathode -> Ground
 * RS -> PA14 (which is now PC11)
 * RW -> PA13 (which is now PC10)
 * E -> PB7
 * D4 -> PD4
 * D5 -> PD5
 * D6 -> PD6
 * D7 -> PD7
 */

/*
 * Setup function for the LCD, should only need to call
 * once at beginning of program
 */
void LCD_Init();

/*
 * Clear screen of LCD
 */
void LCD_Clear();

/*
 * Delay function for the LCD (does a delay, not tested for accuracy)
 */
void LCD_Delay(volatile int time_us);

/*
 * Writes a single character to the LCD
 */
void LCD_WriteChar(char ch);

/*
 * Writes a string to the LCD of max length n
 */
void LCD_WriteString(char *s, int n);

/*
 * Clears the display and writes a string to the LCD 
 * of max length n
 */
void LCD_OverWriteString(char *s, int n);

/*
 * Function moves the LCD cursor to a specific spot on the
 * screen
 */
void LCD_GoToXY(int x, int y);

/*
 * Converts a given voltage into a string and stores
 * it in the provided string, ready for output
 */
int LCD_VoltToStr(int voltage, char *str);

#endif