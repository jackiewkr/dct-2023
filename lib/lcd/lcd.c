#include "stm32f4xx_hal.h"
#include "lcd.h"

/*
 * Configures a specified pin in the GPIO port as an input that is suitable 
 * for reading. Just provide the pin number and port name as parameters
 */
void LCD_Set_As_Input(int pin, GPIO_TypeDef *port) {
    // Setting MODER to input
    // Setting PUPDR to no pull up / pull down
    unsigned long mask = ~(3UL << 2 * pin);
    port->MODER &= mask;
    port->PUPDR = (port->PUPDR & mask) | (0UL << 2 * pin);
}

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
 * Delay function for the LCD (timing is nowhere near exact and not tested)
 */
void LCD_Delay(volatile int time_us) {
    // Factor between the system core clock (believe 16MHz) and 16MHz
    float i = (float) SystemCoreClock / (float) 16e6;
    volatile unsigned long count = (unsigned long) (i * (36 * time_us >> 4));
    while (--count > 0);
}

/*
 * Reads and returns the value of a pin configured as an
 * input
 */
uint8_t LCD_Read_Pin(int pin, GPIO_TypeDef *port) {
    return (port->IDR & 1 << pin) >> pin;
}

/*
 * Reads the value of PD7 to see if the LCD is currently 
 * in the middle of an internal operation
 */
uint8_t LCD_Is_Busy() {
    uint8_t busy;

    // Setting the data lines to be inputs for reading
    for (int loop = 0; loop < 8; ++loop)
        LCD_Set_As_Input(loop, GPIOD);
    LCD_Delay(100);
    
    // Selecting the instruction register on the LCD
    // and setting the LCD to read mode
    GPIOA->BSRR = 1UL << 30;
    GPIOA->BSRR |= 1UL << 13;

    // Setting E high, which behaves like a clock signal
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 7;

    // Reading the value on PD7. If it is a 1 then the LCD is busy
    LCD_Delay(100);
    busy = LCD_Read_Pin(7, GPIOD);

    // Toggling E up and down a couple of times
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 23;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 7;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 23;

    // Setting the ports back to being outputs
    for (int loop = 0; loop < 8; ++loop)
        LCD_Set_As_Output(loop, GPIOD);
    LCD_Delay(100);

    // Setting the LCD back to write mode
    GPIOA->BSRR = 1UL << 29;
    LCD_Delay(100);

    return busy;
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

    // Waiting for the LCD to be ready before we move on
    while (LCD_Is_Busy());

    // Sending a signal to E to clear it
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 7;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 23;

    // Writing the instruction to put the LCD into 4 bit mode
    GPIOD->ODR = 0x20;

    // Sending a signal to E to put the instruction through
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 7;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 23;
}
