#include "stm32f4xx_hal.h"
#include "lcd.h"

/*
 * Setup function for the LCD
 */
void LCD_Init() {
    // Initialising the GPIO clocks
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIODEN;
}
