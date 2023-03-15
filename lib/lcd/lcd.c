#include "stm32f4xx_hal.h"
#include "lcd.h"

/*
 * Configures a specified pin in the GPIO port as a general purpose, push-pull
 * output. Just provide the pin number and port name as parameters
 */
void LCD_Set_As_Output(int pin, GPIO_TypeDef *port) {
    // Setting MODER to general purpose output mode
    // Setting OTYPER to push pull output mode
    // Setting PUPDR to no pull up / pull down
    unsigned long mask = ~(3UL << 2 * pin);
    port->MODER = (port->MODER & mask) | (1UL << 2 * pin);
    port->OTYPER &= ~(1UL << pin);
    port->PUPDR = (port->PUPDR & mask) | (0UL << 2 * pin);
}

/*
 * Setup function for the LCD
 */
void LCD_Init() {
    // Initialising the GPIO clocks
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /*
     * Setting our control lines to be outputs. 
     * PA14 - Connected to RS (default logic low)
     * PA13 - Connected to RW (default logic low)
     * PB7 - Connected to E (default logic low)
     */
    LCD_Set_As_Output(14, GPIOA);
    LCD_Set_As_Output(13, GPIOA);
    LCD_Set_As_Output(7, GPIOB);

    // Setting PA14 (RS), PA13 (RW), PB7 (E) to logic low
    GPIOA->BSRR = 1UL << 30;
    GPIOA->BSRR = 1UL << 29;
    GPIOB->BSRR = 1UL << 23;

    // Setting the data lines PD4, PD5, PD6, PD7 to outputs
    for (int loop = 0; loop < 8; ++loop)
        LCD_Set_As_Output(loop, GPIOD);
}
