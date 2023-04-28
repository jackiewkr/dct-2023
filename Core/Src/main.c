#include "main.h"
#if 0 //switch to 0 if working using lab daughterboards
    #include "lcd.h"
#else
    #include "PB_LCD_Drivers.h"
#endif
#include "adc.h"
#include "button.h"

#include <stdio.h>

#define        TEST ( 0 )
#define  RESISTANCE ( 1 )
#define CAPACITANCE ( 2 )
#define  INDUCTANCE ( 3 )

struct State
{
    /* TEST = 0, RESISTANCE, CAPACITANCE, INDUCTANCE */
    uint8_t state; //what's being measured
    
    /* 1K = 0, 10K, 100K, 1M for resistors*/
    /* 1n = 0, 10n, 100n, 1u for capacitors */
    /* 100u = 0, 1m for inductors */
    uint8_t range; //range of current measurement
    
    struct ADC_Output output;
};

/** Prototypes **/
void SystemClock_Config(void);
void printTest( struct State );
void printResistance( struct State );
void printCapacitance( struct State );
void printInductance( struct State );

void outputLogic0( void )
{
    //A
    GPIOA->BSRR = 1<<(4+16);
    GPIOA->BSRR = 1<<(5+16);
    //B
    GPIOA->BSRR = 1<<(6+16);
    GPIOA->BSRR = 1<<(7+16);
}

void outputLogic1( void )
{
    //A
    GPIOA->BSRR = 1<<(4);
    GPIOA->BSRR = 1<<(5);
    //B
    GPIOA->BSRR = 1<<(6+16);
    GPIOA->BSRR = 1<<(7+16);
}

void outputLogic2( void )
{
    //A
    GPIOA->BSRR = 1<<(4+16);
    GPIOA->BSRR = 1<<(5+16);
    //B
    GPIOA->BSRR = 1<<(6);
    GPIOA->BSRR = 1<<(7);
}

int main(void)
{
    /* Initialize HAL libraries */
    HAL_Init();
    SystemClock_Config();

    /* Initialize user libraries */
    ADC_Init();
    PB_LCD_Init();
    Button_init();
    
    /* Initialize logic pins PA4, PA5, PA6, PC4, PC5 */
    GPIOA->MODER |= GPIO_MODER_MODER4_0;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;
    GPIOA->MODER |= GPIO_MODER_MODER6_0;
    GPIOA->MODER |= GPIO_MODER_MODER7_0;
    
    /* Simple Bootscreen */
    PB_LCD_Clear();
    PB_LCD_WriteString( "Loading...", 13 );
    
    struct State state;
    state.state = TEST;
    outputLogic0();
    
    /* Main program loop */
    state.output = ADC_Measure();
    while (1)
    {
        HAL_Delay(1000);
        PB_LCD_Clear();
        
        //state.output = ADC_Measure();
        printTest( state );
        
        #if 0
        if ( state.state == TEST )
        {
            printTest( state );
        } else if ( state.state == RESISTANCE )
        {
            printResistance( state );
        } else if ( state.state == CAPACITANCE )
        {
            printCapacitance( state );
        } else if ( state.state == INDUCTANCE )
        {
            printInductance( state );
        } else
        {
            state.state = TEST;
            printTest( state );
        }
        #endif
    }
}

void printTest( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 'T' );
    
    PB_LCD_GoToXY( 0,0 );
    
    char top_buf[15];
    snprintf( top_buf, 15, "A:%dm B:%dm", st.output.v_a, st.output.v_b );
    
    char bot_buf[16];
    snprintf( bot_buf, 16, "C:%dm", st.output.v_c );
    
    PB_LCD_WriteString( top_buf, 15 );
    PB_LCD_GoToXY(0,1);
    PB_LCD_WriteString( bot_buf, 16 );
}

void printResistance( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 222 );
    
    PB_LCD_GoToXY( 0,0 );
}

void printCapacitance( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 'F' );
    
    PB_LCD_GoToXY( 0,0 );
}

void printInductance( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 'H' );
    
    PB_LCD_GoToXY( 0,0 );
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}
