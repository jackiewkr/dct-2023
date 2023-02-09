#ifndef __ADC_H
#define __ADC_H
/**
 * Simple ADC library. Uses only 1 channel on 1 ADC at the moment.
 * To input, connect analog source to pin PA1.
 **/

void ADC_init( void );

double ADC_getVoltage( void );

#endif //__ADC_H