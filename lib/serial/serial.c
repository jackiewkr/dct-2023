#include "stm32f4xx.h"

#include "serial.h"
/* Copy of dajp1's serial library */

void SERIAL_init() {
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable UART2 clock
        
        // Set up baud rate for the UART as a division from APB1
        // clock rate to four binary places. For example, for 9600
        // baud from a 16 MHz APB1 clock, setting BRR to 104 gives
        // a baud rate of (16e6 / 104) >> 4 = 9615.4 baud. This is
        // less than 0.2% out, and it’s close enough for serial
        // communications.
        USART2->BRR = 104 << 4; // 9600 baud with 16 MHz APB1 clock
        USART2->CR1 = (1UL << 13) | (1UL << 3);
        
        // Turn clock to GPIOA on:
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
        
        // Set-up PA2 as an output, and configure PA2 to take input
        // from USART2 (alternate function mode):
        GPIOA->AFR[0] = (GPIOA->AFR[0] & 0xFFFFF0FF) | (0x7 << 8);
        
        unsigned int bit = 2;
        unsigned long bitMask = ~(3UL << 2*bit);
        GPIOA->MODER = (GPIOA->MODER & bitMask) | (2UL << 2*bit);
}

void SERIAL_send(char *bytes, int howMany) {
        // Sends howMany bytes, starting at the address pointed to by
        // bytes, or until it reaches the end of the string
        // (whichever happens first).
        unsigned int next = 0;
        while (next++ < howMany && *bytes) {
                while (!(USART2->SR & USART_SR_TXE));
                USART2->DR = *bytes++;
        }
}

void SERIAL_send_newline() {
        // Does what it says on the tin... sends a CR and LF:
        while (!(USART2->SR & USART_SR_TXE)); // Wait until ready
        USART2->DR = '\r';
        while (!(USART2->SR & USART_SR_TXE));
        USART2->DR = '\n';
}

