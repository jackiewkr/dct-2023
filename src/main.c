/* STM inbuilt library */
#include "stm32f4xx.h"

/* User Libraries */
#include "led.h"
#include "adc.h"

/* Simple delay function using NOP */
void delay_ms( int ms );

/* MAIN PROGRAM */
int main( void )
{
        /* Test program for checking if the ADC works.
         * Check using Keil's inbuilt variable viewer */
        double voltage = 0;
        int cntr = 0;
        
        /* Initialize libraries */
        LED_init();
        ADC_init();
        
        LED_on( 3 );
        while (1)
        {
                delay_ms( 100 );
                LED_off( 3 );
                

                voltage = ADC_getVoltage();
                LED_on( 3 );
                cntr++;
        }
        
        /*return 0;*/
}

void delay_ms( volatile int ms )
{
        int j;
        for ( j = 0; j < ms * 4000; j++ );
}
