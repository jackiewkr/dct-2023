#ifndef __ADC_H
#define __ADC_H
/**
 * adc.h
 * =====
 * Simple ADC library that reads voltage from pin PC5 on the STM32F4 DISCO board
 **/

#include "stm32f4xx_hal.h"

/* Initialize the ADC */
void ADC_Init( void );

/* Returns the voltage in millivolts */
uint16_t ADC_Measure( void );

#endif /* __ADC_H */
