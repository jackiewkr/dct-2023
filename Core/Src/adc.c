#include "stm32f4xx_hal.h"
#include "adc.h"

static ADC_HandleTypeDef hadc1;

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
void ADC_Init(void)
{
	GPIO_Init();

  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc1);

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}



/* Raw voltage btwn 0-4096, so divide by 4096 and multiply btwn 3300 to get
 * voltage in millivolts */
static uint16_t ADC_RawToMillivolt( uint16_t raw )
{
        return ( raw * 3300 ) / 4096;
}

/* Measures analog value on pin PC5 */
uint16_t ADC_Measure( void )
{
	HAL_ADC_Start( &hadc1 );
	HAL_ADC_PollForConversion( &hadc1, HAL_MAX_DELAY );
        
        uint16_t total = HAL_ADC_GetValue( &hadc1 );
        
	HAL_ADC_Stop( &hadc1 );
	return ADC_RawToMillivolt( total );
}
