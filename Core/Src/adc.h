#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include "stm32f4xx_hal.h"

struct ADC_Output
{
	uint16_t v_a;
	uint16_t v_b;
	uint16_t v_c;
};

void ADC_Init( void );

struct ADC_Output ADC_Measure( void );

#endif
