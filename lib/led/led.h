#ifndef __LED_H
#define __LED_H
/**
 * led.h
 * =====
 * Simple LED library for controlling the onboard LEDs on the STM32F4 DISCO
 * board.
 **/

/* Initialize the LED library */
void LED_init( void );

void LED_on( int num );
void LED_off( int num );

#endif /* __LED_H */
