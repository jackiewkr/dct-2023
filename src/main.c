/* STM inbuilt library */
#include "stm32f4xx_hal.h"

/* User libraries */
#include <adc.h>

/* glibc libraries */
#include <stdint.h>

void Init_OnBoard_LEDs(void);
void Delay_ms(volatile int time_ms);

int main() {
        double voltage;
        
	Init_OnBoard_LEDs();
        //ADC for resistor calcs
        struct ADC* adc1 = ADC_Init( 1 );
        
	while( 1 )
	{
	        voltage = ADC_GetVoltage( adc1 );
                if ( voltage >= 1 )
                {
                        //if greater than threshold, blink LED for now
                        HAL_GPIO_TogglePin( GPIOD, GPIO_PIN_12 );
                        Delay_ms( 200 );
                        HAL_GPIO_TogglePin( GPIOD, GPIO_PIN_12 );
                }
	}

        ADC_Free( adc1 );
}

void Init_OnBoard_LEDs() {
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef BoardLEDs;
	BoardLEDs.Mode = GPIO_MODE_OUTPUT_PP;
	BoardLEDs.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &BoardLEDs);
}

void Delay_ms(volatile int time_ms) {
	int j;
    for (j = 0; j < time_ms*4000; j++)
        {}  /* excute NOP for 1ms */
}
