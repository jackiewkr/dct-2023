#include "stm32f4xx_hal.h"

// math.h is used for sin() function to generate wave LUT's
#include <math.h>

#define WAVEFORM_1K_SIZE 256 //number of samples in the 1khz waveform
#define WAVEFORM_10K_SIZE 64 //number of samples in the 10khz waveform
#define WAVEFORM_100K_SIZE 16 //number of samples in the 100khz waveform
#define WAVE_AMPLITUDE 620 //Amplitude of the sine wave as value for DAC
#define WAVE_OFFSET 620 //Offset of the sine wave as value for the DAC
#define TWO_PI 6.28318530718f

// LUTs used for the 1khz, 10khz, 100khz sine waves respectively
uint32_t waveform1kdata[WAVEFORM_1K_SIZE];
uint32_t waveform10kdata[WAVEFORM_10K_SIZE];
uint32_t waveform100kdata[WAVEFORM_100K_SIZE];

// Function is used to initialise the LUTs for the different waveforms
void LUT_Init() {
    float dac_val, sine_val;

    for (int i = 0; i < WAVEFORM_1K_SIZE; ++i) {
        sine_val = WAVE_AMPLITUDE * sin(i * TWO_PI / WAVEFORM_1K_SIZE);
        dac_val = WAVE_OFFSET + sine_val;
        waveform1kdata[i] = dac_val;
    }

    for (int i = 0; i < WAVEFORM_10K_SIZE; ++i) {
        sine_val = WAVE_AMPLITUDE * sin(i * TWO_PI / WAVEFORM_10K_SIZE);
        dac_val = WAVE_OFFSET + sine_val;
        waveform10kdata[i] = dac_val;
    }

    for (int i = 0; i < WAVEFORM_100K_SIZE; ++i) {
        sine_val = WAVE_AMPLITUDE * sin(i * TWO_PI / WAVEFORM_100K_SIZE);
        dac_val = WAVE_OFFSET + sine_val;
        waveform100kdata[i] = dac_val;
    }
}

// Function is used to initialise the DAC for of sine wave generation using DMA
void DAC_Init() {
    // Enabling the clocks for the GPIOA, DAC, DMA and Timer 5
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    // Setting up PA4 to be an analogue output
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE4_Msk) | (0x3 << GPIO_MODER_MODE4_Pos);

    // Set up the timer (initially for a 1khz wave)
    TIM5->PSC = 0;
    TIM5->ARR = 9;
    TIM5->DIER |= TIM_DIER_UDE;

    // Set up DMA to transfer data from LUTs to the DAC
    DMA1_Stream6->PAR = (uint32_t) &DAC1->DHR12R1;
    DMA1_Stream6->M0AR = (uint32_t) waveform100kdata;
    DMA1_Stream6->NDTR = WAVEFORM_100K_SIZE;

    // Set channel on DMA to channel 6 to use timer 5 
    DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_CHSEL_Msk) | (0x6 << DMA_SxCR_CHSEL_Pos);

    // Set priority on the DMA to very high
    DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_PL_Msk) | (0x3 << DMA_SxCR_PL_Pos);

    // Set DMA to increment memory address, and set circular mode
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_PINC_Msk);
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_MINC_Msk) | (0x1 << DMA_SxCR_MINC_Pos);
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_CIRC_Msk) | (0x1 << DMA_SxCR_CIRC_Pos);

    // Set data direction of the DMA to memory to peripheral
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_DIR_Msk) | (0x1 << DMA_SxCR_DIR_Pos);

    // Set the size of the data transfer to be 32-bit
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_MSIZE_Msk) | (0x2 << DMA_SxCR_MSIZE_Pos);
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_PSIZE_Msk) | (0x2 << DMA_SxCR_PSIZE_Pos);

    // Enable the DMA controller
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_EN_Msk) | (0x1 << DMA_SxCR_EN_Pos);

    // Set up the DAC
	DAC->CR = (DAC->CR & ~DAC_CR_EN1) | (0x1 << DAC_CR_EN1_Pos);

    // Enable the timer
	TIM5->CR1 |= 0x01;
}

// Function is used to get the DAC to output a 1khz sine wave on PA4
void DAC_Output1kWave() {
    // Disable the DMA controller, DAC and timer so we can change values
    DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_EN_Msk);
    DAC->CR = (DAC->CR & ~DAC_CR_EN1);
    TIM5->CR1 ^= 0x01;

    // Changing prescaler and ARR to relevant values for 1k waveform
    TIM5->PSC = 1;
    TIM5->ARR = 30;

    // Setting up DMA to transfer from the 1khz LUT
    DMA1_Stream6->PAR = (uint32_t) &DAC1->DHR12R1;
    DMA1_Stream6->M0AR = (uint32_t) waveform1kdata;
    DMA1_Stream6->NDTR = WAVEFORM_1K_SIZE;

    // Re-enabling the DMA controller
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_EN_Msk) | (0x1 << DMA_SxCR_EN_Pos);

    // Re-enabling the DAC
	DAC->CR = (DAC->CR & ~DAC_CR_EN1) | (0x1 << DAC_CR_EN1_Pos);

    // Re-enabling the timer
	TIM5->CR1 |= 0x01;
}

// Function is used to get the DAC to output a 10khz sine wave on PA4
void DAC_Output10kWave() {
    // Disable the DMA controller, DAC and timer so we can change values
    DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_EN_Msk);
    DAC->CR = (DAC->CR & ~DAC_CR_EN1);
    TIM5->CR1 ^= 0x01;

    // Changing prescaler and ARR to relevant values for 10k waveform
    TIM5->PSC = 0;
    TIM5->ARR = 24;

    // Setting up DMA to transfer from the 10khz LUT
    DMA1_Stream6->PAR = (uint32_t) &DAC1->DHR12R1;
    DMA1_Stream6->M0AR = (uint32_t) waveform10kdata;
    DMA1_Stream6->NDTR = WAVEFORM_10K_SIZE;

    // Re-enabling the DMA controller
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_EN_Msk) | (0x1 << DMA_SxCR_EN_Pos);

    // Re-enabling the DAC
	DAC->CR = (DAC->CR & ~DAC_CR_EN1) | (0x1 << DAC_CR_EN1_Pos);

    // Re-enabling the timer
	TIM5->CR1 |= 0x01;
}

// Function is used to get the DAC to output a 100khz sine wave on PA4
void DAC_Output100kWave() {
    // Disable the DMA controller, DAC and timer so we can change values
    DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_EN_Msk);
    DAC->CR = (DAC->CR & ~DAC_CR_EN1);
    TIM5->CR1 ^= 0x01;

    // Changing prescaler and ARR to relevant values for 100k waveform
    TIM5->PSC = 0;
    TIM5->ARR = 9;

    // Setting up DMA to transfer from the 100khz LUT
    DMA1_Stream6->PAR = (uint32_t) &DAC1->DHR12R1;
    DMA1_Stream6->M0AR = (uint32_t) waveform100kdata;
    DMA1_Stream6->NDTR = WAVEFORM_100K_SIZE;

    // Re-enabling the DMA controller
	DMA1_Stream6->CR = (DMA1_Stream6->CR & ~DMA_SxCR_EN_Msk) | (0x1 << DMA_SxCR_EN_Pos);

    // Re-enabling the DAC
	DAC->CR = (DAC->CR & ~DAC_CR_EN1) | (0x1 << DAC_CR_EN1_Pos);

    // Re-enabling the timer
	TIM5->CR1 |= 0x01;
}