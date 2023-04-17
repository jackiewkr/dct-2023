/**
 * adc.c
 * =====
 * Library for using an ADC of the STM32F4 DISCO boards.
 *
 * Measures from 0 - 3.3V
 **/

#include "stm32f4xx_hal.h"
#include "adc.h"

static ADC_HandleTypeDef hadc1; //ADC Handler

static void ADC_SetupPin(void)
{
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        __HAL_RCC_GPIOC_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init( GPIOC, GPIO_InitStruct );
}

void ADC_Init(void)
{
        ADC_SetupPin();
        
        ADC_ChannelConfTypeDef sConfig = {0};

        /* configure adc1 features */
        hadc1.Instance = ADC1;
        hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
        hadc1.Init.Resolution = ADC_RESOLUTION_12B;
        hadc1.Init.ScanConvMode = DISABLE;
        hadc1.Init.ContinuousConvMode = ENABLE;
        hadc1.Init.DiscontinuousConvMode = DISABLE;
        hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        hadc1.Init.NbrOfConversion = 1;
        hadc1.Init.DMAContinuousRequests = DISABLE;
        hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        
        HAL_ADC_Init(&hadc1);

        /* configure ADC channel to channel 15*/
        sConfig.Channel = ADC_CHANNEL_15;
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
        
        HAL_ADC_ConfigChannel(&hadc1, &sConfig)

}

/* Raw voltage btwn 0-4096, so divide by 4096 and multiply btwn 3300 to get
 * voltage in millivolts */
static uint16_t ADC_RawToMillivolt( uint16_t raw )
{
        return ( raw / 4096 ) * 3300;
}

/* Measures analog value on pin PC5 */
uint16_t ADC_Measure( void )
{
	HAL_ADC_Start( hadc1 );
	HAL_ADC_PollForConversion( hadc1, HAL_MAX_DELAY );
        
        uint16_t total = HAL_ADC_GetValue( hadc1 );
        
	HAL_ADC_Stop( hadc1 );
	return total;
}
