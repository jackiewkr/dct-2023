#include "stm32f4xx.h"

#include "adc.h"
/** 
 * Simple ADC register-level library for reading from PA1 (CHANNEL 1) using
 * ADC1.
 * ADC is set up in single conversion mode for simplicity.
 **/

static void _setup_GPIO( void )
{
        //enable GPIOA clock 
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

        //setup GPIOA moder to analogue input for pin 1 
        GPIOA->MODER &= ~GPIO_MODER_MODER1_Msk;
        GPIOA->MODER |= (3<<GPIO_MODER_MODER1_Pos);
}

static void _setup_ADC1( void )
{
        /* Enable ADC1 clock */
        RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN_Msk;
        RCC->APB2ENR |= (1<<RCC_APB2ENR_ADC1EN_Pos);

        /* Set ADC Control Registers 1 & 2 to default states */
        ADC1->CR1 = (1<<ADC_CR1_DISCEN_Pos); //DISCEN starts high
        ADC1->CR2 = (1<<ADC_CR2_EOCS_Pos); //EOCS starts high

        /* Set ADC to single conversion mode */
        ADC1->SQR1 &= ~ADC_SQR1_L_Msk;
        ADC1->SQR1 |= (0<<ADC_SQR1_L_Pos); //set L to '0000'

        /* Set ADC channel to CHANNEL 1 (PA1) */
        ADC1->SQR3 &= ~ADC_SQR3_SQ1_Msk;
        ADC1->SQR3 |= (ADC_SQR3_SQ1_1<<ADC_SQR3_SQ1_Pos); //set ADC1 chan to 1

        /* Enable ADC1 */
        ADC1->CR2 |= (ADC_CR2_ADON<<ADC_CR2_ADON_Pos);
}

void ADC_init( void )
{
        /* Setup GPIO pin PA1 */
        _setup_GPIO();

        /* Setup ADC1 */
        _setup_ADC1();
}

double ADC_getVoltage( void )
{
        int raw_voltage;
        /* Set SWSTART high and wait until end of conversion */
        ADC1->CR2 |= (ADC_CR2_SWSTART<<ADC_CR2_SWSTART_Pos);
        while (ADC1->SR & (1<<ADC_SR_EOC_Pos) != 1 )
        {} //wait

        /* Read converted value from bottom half od ADC_DR */
        raw_voltage = ADC1->DR;

        //do voltage conversion????
        return (double)raw_voltage;
}
