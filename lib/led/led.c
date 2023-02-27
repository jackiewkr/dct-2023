/**
 * led.c
 * =====
 * Simple LED library for controlling the onboard LEDs on the STM32F4 DISCO
 * board.
 **/

#include "stm32f4xx.h"
#include "led.h"

/*
 * LED to GPIO pin mapping:
 * LD3 -> GPIOD 13
 * LD4 -> GPIOD 12
 * LD5 -> GPIOD 14
 * LD6 -> GPIOD 15
 */

/* Initialize all the LED's pins as outputs */
void LED_init( void )
{
        /* enable GPIOD clock */
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

        /* set user LED pins to mode output */
        GPIOD->MODER |= GPIO_MODER_MODER12_0; /*LD4; green*/
        GPIOD->MODER |= GPIO_MODER_MODER13_0; /*LD3; orange*/
        GPIOD->MODER |= GPIO_MODER_MODER14_0; /*LD5; red*/
        GPIOD->MODER |= GPIO_MODER_MODER15_0; /*LD6; blue*/
}

/* Set the given LED on */
void LED_on( int num )
{
        switch (num)
        {
        case 3:
                GPIOD->BSRR = 1<<13;
                break;
        case 4:
                GPIOD->BSRR = 1<<12;
                break;
        case 5:
                GPIOD->BSRR = 1<<14;
                break;
        case 6:
                GPIOD->BSRR = 1<<15;
                break;
        default:
                break;
        }
}

/* Turns the given LED off */
void LED_off( int num )
{
        switch (num)
        {
        case 3:
                GPIOD->BSRR = 1<<(13+16);
                break;
        case 4:
                GPIOD->BSRR = 1<<(12+16);
                break;
        case 5:
                GPIOD->BSRR = 1<<(14+16);
                break;
        case 6:
                GPIOD->BSRR = (unsigned int)1<<(15+16);
                break;
        default:
                break;
        }
}
