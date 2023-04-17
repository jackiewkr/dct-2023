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
        LCD_Init();
        ADC_Init();

        LCD_Clear();

        char* buf = NULL;
        while (1)
        {
                delay_ms( 100 );

                LCD_Clear();
                
                voltage = ADC_Measure();
                int buf_sz = LCD_VoltToStr( voltage, buf );
                LCD_WriteString( buf, buf_sz );
        }
        
        /*return 0;*/
}

void delay_ms( volatile int ms )
{
        int j;
        for ( j = 0; j < ms * 4000; j++ );
}
