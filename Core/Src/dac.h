#ifndef DAC_H_INCLUDED
#define DAC_H_INCLUDED

// Function is used to initialise the LUTs for the different waveforms
void LUT_Init();

// Function is used to initialise the DAC for of sine wave generation using DMA
void DAC_Init();

// Function is used to get the DAC to output a 1khz sine wave on PA4
void DAC_Output1kWave();

// Function is used to get the DAC to output a 10khz sine wave on PA4
void DAC_Output10kWave();

// Function is used to get the DAC to output a 100khz sine wave on PA4
void DAC_Output100kWave();

#endif