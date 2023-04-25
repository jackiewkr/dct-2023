#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include "stm32f4xx_hal.h"

void ADC_Init( void );

uint16_t ADC_Measure( void );

#endif
