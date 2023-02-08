/* STM inbuilt library */
#include "stm32f4xx.h"

/* User libraries */
#include <led.h>

/* glibc libraries */

/** Function declarations **/
void delay_ms( int ms );

int main() {
        LED_init();
        while (1)
        {
                delay_ms(200);
                LED_on(3);
                LED_on(4);
                LED_on(5);
                LED_on(6);

                delay_ms(200);
                LED_off(3);
                LED_off(4);
                LED_off(5);
                LED_off(6);
        }
}

void delay_ms( int ms )
{
        int j;
        for (j = 0; j < ms*4000; j++)
        {}  /* excute NOP for 1ms */
}

