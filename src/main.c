/* STM inbuilt library */
#include "stm32f4xx.h"

/* User libraries */
#include "led.h"
#include "serial.h"
#include "adc.h"

/* glibc libraries */
#include <stdio.h>

/** Function declarations **/
void delay_ms( int ms );

int main() {
        /* Initialize all libraries */
        LED_init();
        ADC_init();
        SERIAL_init();

        int voltage;
        char buf[32];
        while (1)
        {
                delay_ms( 100 );
                voltage = ADC_getVoltage();

                snprintf( buf, 32, "%f", voltage );
                SERIAL_send( buf, 32 );
        }
}

void delay_ms( volatile int ms )
{
        int j;
        for (j = 0; j < ms*4000; j++)
        {}  /* excute NOP for 1ms */
}

