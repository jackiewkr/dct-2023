#include "stm32f4xx_hal.h"

#include <stdint.h>                                                                                                                

void Init_OnBoard_LEDs(void);
void Init_GPIO( void );
ADC_HandleTypeDef configure_ADC2_Channel_0( void );
void Delay_ms(volatile int time_ms);

int main() {
	uint32_t raw_voltage;
	ADC_HandleTypeDef myADC2Handle;
	double voltage;
	Init_GPIO(); // initialize PA0 pin 
	myADC2Handle = configure_ADC2_Channel_0(); // configure ADC2
	Init_OnBoard_LEDs();
        
	while(1)
	{
		HAL_ADC_Start(&myADC2Handle); // start A/D conversion
		if(HAL_ADC_PollForConversion(&myADC2Handle, 5) == HAL_OK) //check if conversion is completed
		{
                        raw_voltage = HAL_ADC_GetValue(&myADC2Handle); // read digital value and save it inside uint32_t variable
                        voltage = ( raw_voltage * 8 ) / 10000;

                        if ( voltage >= 1 ) {
                                HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12);
                                Delay_ms(200);
								HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12);
                        }
		}
		HAL_ADC_Stop(&myADC2Handle); // stop conversion 
		Delay_ms(200);
	}
}

void Init_OnBoard_LEDs() {
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef BoardLEDs;
	BoardLEDs.Mode = GPIO_MODE_OUTPUT_PP;
	BoardLEDs.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &BoardLEDs);
}

void Init_GPIO( void ) {
	__HAL_RCC_GPIOA_CLK_ENABLE(); // enable clock to GPIOA
	GPIO_InitTypeDef ADCpin; //create an instance of GPIO_InitTypeDef
	ADCpin.Pin = GPIO_PIN_0; // Select pin PA0
	ADCpin.Mode = GPIO_MODE_ANALOG; // Select Analog Mode
	ADCpin.Pull = GPIO_NOPULL; // Disable internal pull-up or pull-down resistor
	HAL_GPIO_Init(GPIOA, &ADCpin); // initialize PA0 as analog input pin
}

ADC_HandleTypeDef configure_ADC2_Channel_0(void)
{
	ADC_HandleTypeDef myADC2Handle;
    __HAL_RCC_ADC2_CLK_ENABLE(); // enable clock to ADC2 module
	myADC2Handle.Instance = ADC2; // create an instance of ADC2
	myADC2Handle.Init.Resolution = ADC_RESOLUTION_12B; // select 12-bit resolution 
	myADC2Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV; //select  single conversion as a end of conversion event
	myADC2Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // set digital output data right justified
	myADC2Handle.Init.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV8; 
	HAL_ADC_Init(&myADC2Handle); // initialize AD2 with myADC2Handle configuration settings
	
    /*select ADC2 channel */
	ADC_ChannelConfTypeDef Channel_AN0; // create an instance of ADC_ChannelConfTypeDef
	Channel_AN0.Channel = ADC_CHANNEL_0; // select analog channel 0
	Channel_AN0.Rank = 1; // set rank to 1
	Channel_AN0.SamplingTime = ADC_SAMPLETIME_15CYCLES; // set sampling time to 15 clock cycles
	HAL_ADC_ConfigChannel(&myADC2Handle, &Channel_AN0); // select channel_0 for ADC2 module. 
	return myADC2Handle;
}

void Delay_ms(volatile int time_ms) {
	int j;
    for (j = 0; j < time_ms*4000; j++)
        {}  /* excute NOP for 1ms */
}