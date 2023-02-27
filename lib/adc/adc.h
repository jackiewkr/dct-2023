#ifndef __ADC_H
#define __ADC_H
/**
 * adc.h
 * =====
 * Simple ADC library that reads voltage from pin PC5 on the STM32F4 DISCO board
 **/

/* Initialize the ADC */
void ADC_init( void );

/* Returns the voltage in millivolts */
unsigned int ADC_getVoltage( void );

#endif /* __ADC_H */
