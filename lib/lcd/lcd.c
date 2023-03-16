#include "stm32f4xx_hal.h"
#include "lcd.h"

enum eLCD_OP {INSTRUCTION, DATA};

/*
 * Simple swap function for swapping two chars
 */
void swap(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

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
    GPIOC->BSRR = 1UL << 27;
    GPIOC->BSRR |= 1UL << 10;

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
    GPIOC->BSRR = 1UL << 26;
    LCD_Delay(100);

    return busy;
}

/*
 * Function for writing a byte to the LCD
 */
void LCD_Write(enum eLCD_OP opcode, uint8_t data) {
    // Waiting for the LCD to be ready
    while (LCD_Is_Busy());

    // Setting RS according to the opcode
    if (opcode == DATA)
        GPIOC->BSRR = 1UL << 11;
    else if (opcode == INSTRUCTION)
        GPIOC->BSRR = 1UL << 27;
    else 
        return;

    // Creating variables for the upper and lower part of the byte
    unsigned int upper_bits = data & 0xF0;
    unsigned int lower_bits = (data << 4) & 0xF0;

    // Writing the upper bits to the LCD
    GPIOD->ODR = upper_bits;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 7;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 23;

    // Writing the lower bits to the LCD
    GPIOD->ODR = lower_bits;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 7;
    LCD_Delay(100);
    GPIOB->BSRR = 1UL << 23;
}

/*
 * 
 * HEADER FILE FUNCTIONS BELOW
 * 
 */

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
 * Setup function for the LCD
 */
void LCD_Init() {
    // Initialising the GPIO clocks
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC-> AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /*
     * Setting our control lines to be outputs. 
     * PC11 - Connected to RS (default logic low)
     * PC10 - Connected to RW (default logic low)
     * PB7 - Connected to E (default logic low)
     */
    LCD_Set_As_Output(11, GPIOC);
    LCD_Set_As_Output(10, GPIOC);
    LCD_Set_As_Output(7, GPIOB);

    // Setting PC11 (RS), PC10 (RW), PB7 (E) to logic low
    GPIOC->BSRR = 1UL << 27;
    GPIOC->BSRR = 1UL << 26;
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

    // Setting up the characteristics of the LCD
    // 4 bit mode, 2 lines
    LCD_Write(INSTRUCTION, 0x28);

    // Display on / off control 
    LCD_Write(INSTRUCTION, 0x0C);

    // Increment Entry mode
    LCD_Write(INSTRUCTION, 0x06);

    // Clearing the display
    LCD_Write(INSTRUCTION, 0x01);
}

/*
 * Function for clearing the LCD screen
 */
void LCD_Clear() {
    LCD_Write(INSTRUCTION, 0x01);
}

/*
 * Function for writing a single char to the LCD
 * ASCII values are valid for 0x20 to 0x7E
 */
void LCD_WriteChar(char ch) {
    LCD_Write(DATA, ch);
}

/*
 * Function for writing a full string of n length
 * to the LCD
 */
void LCD_WriteString(char *s, int n) {
    while (*s && n--)
        LCD_Write(DATA, *s++);
}

/*
 * Wrapper function clears the screen and prints a string
 */
void LCD_OverWriteString(char *s, int n) {
    LCD_Write(INSTRUCTION, 0x01);
    while (*s && n--)
        LCD_Write(DATA, *s++);
}

/*
 * Function takes a voltage in millivolts as an integer
 * and converts it to a string, ready to be passed to 
 * WriteString or OverWriteString
 */
void LCD_VoltToStr(int voltage, char *str) {
    int n = 0;
    if (voltage == 0) {
        str[n++] = '0';
    } else {
        while (voltage > 0) {
            str[n++] = (voltage % 10) + '0';
            voltage = voltage / 10;
        }
    }
    str[n] = '\0';

    for (int i = 0; i < n / 2; ++i) {
        swap(&str[i], &str[n - 1 - i]);
    }
}