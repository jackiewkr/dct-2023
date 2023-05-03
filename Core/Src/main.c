#include "main.h"
#include "lcd.h"
//#include "PB_LCD_Drivers.h"
#include "adc.h"
#include "button.h"
#include "dac.h"

#include <stdio.h>

enum eState
{
    TEST = 0,
    RESISTANCE,
    CAPACITANCE,
    INDUCTANCE
};

struct State
{
    enum eState state; //current state of program (i.e. whats being displayed)
    struct ADC_Output output;
};


/** Prototypes **/
void SystemClock_Config(void);
void printTest( struct State );
void printResistance( struct State );
void printCapacitance( struct State );
void printInductance( struct State );

int main(void)
{
    /* Initialize HAL libraries */
    HAL_Init();
    SystemClock_Config();

    /* Initialize DAC for 10k wave */
    LUT_Init();
    DAC_Init();
    DAC_Output10kWave();
    
    /* Initialise ADC */
    ADC_Init();
    
    /* Initialise the LCD */
    LCD_Init();
    
    /* Initialise the Button */
    Button_init();
    
    /* Simple Bootscreen */
    LCD_OverWriteString("Loading...", 13);
    
    struct State state;
    state.state = TEST;
    
    /* Main program loop */
    while (1)
    {
        HAL_Delay(1000);
        LCD_Clear();
        
        state.output = ADC_Measure();
        
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
        
        //if ( Button_isPressed() )
            //state.state++;
    }
}

void printTest( struct State st )
{
    LCD_GoToXY( 15, 0 );
    LCD_WriteChar( 'T' );
    
    LCD_GoToXY( 0,0 );
    
    char top_buf[15];
    snprintf( top_buf, 15, "A:%d B:%d", st.output.v_a, st.output.v_b );
    
    char bot_buf[16];
    snprintf( bot_buf, 16, "C:%d", st.output.v_c );
    
    LCD_WriteString( top_buf, 15 );
    LCD_GoToXY(0,1);
    LCD_WriteString( bot_buf, 16 );
}

void printResistance( struct State st )
{
    LCD_GoToXY( 15, 0 );
    LCD_WriteChar( 244 );
    
    LCD_GoToXY( 0,0 );
}

void printCapacitance( struct State st )
{
    LCD_GoToXY( 15, 0 );
    LCD_WriteChar( 'F' );
  
    LCD_GoToXY( 0,0 );
}

void printInductance( struct State st )
{
    LCD_GoToXY( 15, 0 );
    LCD_WriteChar( 'H' );
    
    LCD_GoToXY( 0,0 );
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
