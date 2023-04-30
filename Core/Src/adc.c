#include "stm32f4xx_hal.h"
#include "adc.h"
#include <math.h>

static ADC_HandleTypeDef hadc1;
static ADC_HandleTypeDef hadc2;
static ADC_HandleTypeDef hadc3;

#define DC_OFFSET_MV ( 500 )
#define PEAK_ITERATIONS ( 3 )
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

static void ADC1_Init( void )
{
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

static void ADC2_Init( void )
{
        ADC_ChannelConfTypeDef sConfig = {0};

         /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
        */
        hadc2.Instance = ADC2;
        hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
        hadc2.Init.Resolution = ADC_RESOLUTION_12B;
        hadc2.Init.ScanConvMode = DISABLE;
        hadc2.Init.ContinuousConvMode = DISABLE;
        hadc2.Init.DiscontinuousConvMode = DISABLE;
        hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        hadc2.Init.NbrOfConversion = 1;
        hadc2.Init.DMAContinuousRequests = DISABLE;
        hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        HAL_ADC_Init(&hadc2);

        /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
        */
        sConfig.Channel = ADC_CHANNEL_1;
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
        HAL_ADC_ConfigChannel(&hadc2, &sConfig);
}

static void ADC3_Init( void )
{
        ADC_ChannelConfTypeDef sConfig = {0};

         /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
        */
        hadc3.Instance = ADC3;
        hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
        hadc3.Init.Resolution = ADC_RESOLUTION_12B;
        hadc3.Init.ScanConvMode = DISABLE;
        hadc3.Init.ContinuousConvMode = DISABLE;
        hadc3.Init.DiscontinuousConvMode = DISABLE;
        hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        hadc3.Init.NbrOfConversion = 1;
        hadc3.Init.DMAContinuousRequests = DISABLE;
        hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        HAL_ADC_Init(&hadc3);

        /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
        */
        sConfig.Channel = ADC_CHANNEL_2;
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
        HAL_ADC_ConfigChannel(&hadc3, &sConfig);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
void ADC_Init(void)
{
	GPIO_Init();

        ADC1_Init();
	ADC2_Init();
	ADC3_Init();
}



/* Raw voltage btwn 0-4096, so divide by 4096 and multiply btwn 3300 to get
 * voltage in millivolts */
static inline uint16_t ADC_RawToMillivolt( uint16_t raw )
{
        return ( raw * 3300 ) / 4096;
}

static inline uint16_t ADC_MillivoltToRMS( uint16_t mv_peak )
{
        return sqrt( DC_OFFSET_MV * DC_OFFSET_MV + ( mv_peak * mv_peak ) / 2 );
}

static uint16_t ADC_GetMeasurement( ADC_HandleTypeDef* adc )
{
	uint16_t output;

	HAL_ADC_Start( adc );
	HAL_ADC_PollForConversion( adc, 1000 );
	output = ADC_RawToMillivolt( ADC_GetValue( adc ) );
	HAL_ADC_Stop( adc );

	return output;
}

/* Measures analog value on pin PC5 */
struct ADC_Output ADC_Measure( void )
{
	struct ADC_Output output;
        uint16_t a_temp = 0;
        uint16_t b_temp = 0;
        uint16_t c_temp = 0;
	int timeout;

        /* adc 1 */
	/* measure adc output multiple times and average out */
        uint16_t average_buf[PEAK_ITERATIONS];
	for ( int i = 0; i < PEAK_ITERATIONS; i++ )
	{
    		timeout = 10;
    		while ( timeout != 0 )
    		{
        		average_buf[i] = ADC_GetMeasurement( &hadc1 );
        		if ( average_buf[i] > a_temp )
			{
            			a_temp = average_buf[i];
            			timeout = 10;
        		} else
        		{
            			timeout--;
        		}
    		}
		average_buf[i] = a_temp;
        }
        /* average out */
        output.v_a = 0;
        for ( int i = 0; i < PEAK_ITERATIONS; i++ )
        {
                output.v_a += average_buf[i];
        }
        output.v_a /= PEAK_ITERATIONS;
	
	
	/* adc 2 */
	/* measure adc output multiple times and average out */
	for ( int i = 0; i < PEAK_ITERATIONS; i++ )
	{
    		timeout = 10;
    		while ( timeout != 0 )
    		{
        		average_buf[i] = ADC_GetMeasurement( &hadc2 );
        		if ( average_buf > b_temp )
			{
            			b_temp = average_buf[i];
            			timeout = 10;
        		} else
        		{
            			timeout--;
        		}
    		}
		average_buf[i] = b_temp;
        }
        /* average out */
        output.v_b = 0;
        for ( int i = 0; i < PEAK_ITERATIONS; i++ )
        {
                output.v_b += average_buf[i];
        }
        output.v_b /= PEAK_ITERATIONS;
        
	
	
	/* adc 3 */
	/* measure adc output multiple times and average out */
	for ( int i = 0; i < PEAK_ITERATIONS; i++ )
	{
    		timeout = 10;
    		while ( timeout != 0 )
    		{
        		average_buf[i] = ADC_GetMeasurement( &hadc3 );
        		if ( average_buf > c_temp )
			{
            			c_temp = average_buf[i];
            			timeout = 10;
        		} else
        		{
            			timeout--;
        		}
    		}
		average_buf[i] = c_temp;
        }
        /* average out */
        output.v_c = 0;
        for ( int i = 0; i < PEAK_ITERATIONS; i++ )
        {
                output.v_c += average_buf[i];
        }
        output.v_c /= PEAK_ITERATIONS;

        /* convert to rms */
        #if 0
	output.v_a = ADC_MillivoltToRMS( output.v_a );
        output.v_b = ADC_MillivoltToRMS( output.v_b );
        output.v_c = ADC_MillivoltToRMS( output.v_c );
        #endif
	
	return output;
}
