/**
 * adc.c
 * =====
 * Library for using an ADC of the STM32F4 DISCO boards.
 * This library sets up ADC1 as a 12-bit single conversion ADC that takes input
 * from channel 15 (linked to pin PC5).
 *
 * Measures from 0 - 3.3V
 **/

#include "stm32f4xx_hal.h"
#include "adc.h"

/* Set as global variable to reduce user complexity */
static ADC_HandleTypeDef adc_handle;

/* Initialize the ADC and it's corresponding analog channel pin */
void ADC_init( void )
{
        /* Configure ADC channel 15's pin as analog input */
        __HAL_RCC_GPIOC_CLK_ENABLE();
        GPIO_InitTypeDef adc_pin;
	adc_pin.Pin = GPIO_PIN_5;
	adc_pin.Mode = GPIO_MODE_ANALOG;
	adc_pin.Pull = GPIO_NOPULL;
	HAL_GPIO_Init( GPIOC, &adc_pin );
        
        /* Configure ADC as single conversion, 12-bit */
        __HAL_RCC_ADC1_CLK_ENABLE();
        adc_handle.Instance = ADC1;
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
	adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_handle.Init.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV8; 
	HAL_ADC_Init( &adc_handle );
	
        /* Set ADC's channel to the pin we configured */
	ADC_ChannelConfTypeDef adc_channel;
	adc_channel.Channel = ADC_CHANNEL_15;
	adc_channel.Rank = 1;
	adc_channel.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	HAL_ADC_ConfigChannel( &adc_handle, &adc_channel );
}

/* Get the voltage in millivolts from pin PC5 using the ADC */
unsigned int ADC_getVoltage( void )
{
        unsigned int raw_voltage = 0;
        
        /* Start ADC conversion and wait until completed */
        HAL_ADC_Start( &adc_handle );
        while ( HAL_ADC_PollForConversion( &adc_handle, 5 ) != HAL_OK )
        {
                /* Get raw ADC value from 0-4096 */
                raw_voltage = HAL_ADC_GetValue( &adc_handle ); 
        }
        /* Stop ADC conversion once we get a value */
        HAL_ADC_Stop( &adc_handle ); 

        /* Returns the raw value converted into millivolts */
        return (raw_voltage * 3300 ) / 4096;
}
