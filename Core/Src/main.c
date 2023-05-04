#include "main.h"
#if 0
	#include "lcd.h"
#else
	#include "PB_LCD_Drivers.h"
#endif
#include "PB_LCD_Drivers.h"
#include "adc.h"
#include "button.h"
#include "dac.h"

#include <stdio.h>

#define        TEST ( 0 )
#define  RESISTANCE ( 1 )
#define CAPACITANCE ( 2 )
#define  INDUCTANCE ( 3 )

struct State
{
    /* TEST = 0, RESISTANCE, CAPACITANCE, INDUCTANCE */
    uint8_t state; //what's being measured
	
		uint32_t impedance;
    
    /* 1K = 0, 10K, 100K, 1M for resistors*/
    /* 1n = 0, 10n, 100n, 1u for capacitors */
    /* 100u = 0, 1m for inductors */
    uint8_t range; //range of current measurement
    
    uint8_t prev_logic_level;
    uint8_t logic_level;
			
    
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
    GPIOA->BSRR = 1<<(5+16);
    //B
    GPIOA->BSRR = 1<<(6+16);
		
		DAC_Output1kWave();
}

void outputLogic1( void )
{
    //A
    GPIOA->BSRR = 1<<(5);
    //B
    GPIOA->BSRR = 1<<(6+16);
}

void outputLogic2( void )
{
    //A
    GPIOA->BSRR = 1<<(5+16);
    //B
    GPIOA->BSRR = 1<<(6);
}

int autoRange( struct State* s )
{
		uint16_t diff_0, diff_1, diff_2;

		outputLogic0();
		s->output = ADC_Measure();
		diff_0 = s->output.v_c - s->output.v_a;
	
		outputLogic1();
		s->output = ADC_Measure();
		diff_1 = s->output.v_c - s->output.v_a;
	
		outputLogic2();
		s->output = ADC_Measure();
		diff_2 = s->output.v_c - s->output.v_a;
	
		if ( diff_0 < diff_1 && diff_0 < diff_2 )
		{
			outputLogic0();
			s->output = ADC_Measure();
			
			return 0;
		} else if ( diff_1 < diff_0 && diff_1 < diff_2 )
		{
			outputLogic1();
			s->output = ADC_Measure();
			
			return 1;
		} else if ( diff_2 < diff_1 && diff_2 < diff_0 )
		{
			outputLogic2();
			s->output = ADC_Measure();
			
			return 2;
		}
}

uint32_t impedanceFromVoltage( uint16_t v_in, uint16_t v_out, int range )
{
		uint32_t resistance;
		switch ( range )
		{
			case 0:
				resistance = 1000;
			break;
			case 1:
				resistance = 10000;
			break;
			case 2:
				resistance = 100000;
			break;
		}
		
		float v_i = v_in / 1000;
		float v_o = v_o / 1000;
		
		uint32_t impedance = - ( resistance * v_i ) / v_o;
		
		return impedance;
}

double capacitanceFromImpedance( uint32_t impedance, int range )
{
		double capacitance;
		uint32_t frequency;
		switch ( range )
		{
			case 0:
				frequency = 1000;
			break;
			case 1:
				frequency = 10000;
			break;
			case 2:
				frequency = 100000;
			break;
		}
		
		capacitance = 1/( 2 * 3.1416 * frequency * impedance );
		
		return capacitance;
}

double inductanceFromImpedance( uint32_t impedance, int range )
{
		double inductance;
		uint32_t frequency;
		switch ( range )
		{
			case 0:
				frequency = 1000;
			break;
			case 1:
				frequency = 10000;
			break;
			case 2:
				frequency = 100000;
			break;
		}
		
		inductance = impedance / ( 2 * 3.1416 * frequency );
		
		return inductance;
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
		LUT_Init();
		DAC_Init();
    
    /* Initialize logic pins PA5, PA6 */
    GPIOA->MODER |= GPIO_MODER_MODER5_0;
    GPIOA->MODER |= GPIO_MODER_MODER6_0;
    
    /* Simple Bootscreen */
    PB_LCD_Clear();
    PB_LCD_WriteString( "Loading...", 13 );
    
    struct State state;
    state.state = TEST;
    state.logic_level = 0;
    state.prev_logic_level = 0;
    
    /* Main program loop */
    state.output = ADC_Measure();
		DAC_Output1kWave();
    while (1)
    {
        HAL_Delay(1000);
        PB_LCD_Clear();
			
				state.prev_logic_level = autoRange( &state );
				if ( state.prev_logic_level != state.logic_level )
						state.logic_level = state.prev_logic_level;
				
				state.impedance = impedanceFromVoltage( state.output.v_a, state.output.v_c, state.logic_level );
        
				/* print display */
				if ( state.state == TEST )
						printTest( state );
				else if ( state.state == RESISTANCE )
						printResistance( state );
				else if ( state.state == CAPACITANCE )
						printCapacitance( state );
				else
						printInductance( state );
        
				
        
				/* if button pressed, get new measurement */
				if ( Button_isPressed() )
				{
						state.state++;
						if ( state.state > INDUCTANCE)
							state.state = 0;
				}
    }
}

void printTest( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 'T' );
    
    PB_LCD_GoToXY( 0,0 );
    
    char top_buf[15];
    snprintf( top_buf, 15, "A:%d B:%d", st.output.v_a, st.output.v_b );
    
    char bot_buf[16];
		snprintf( bot_buf, 16, "C:%d I:%d", st.output.v_c, st.impedance );
    
    PB_LCD_WriteString( top_buf, 15 );
    PB_LCD_GoToXY(0,1);
    PB_LCD_WriteString( bot_buf, 16 );
	
		/* print range */
		PB_LCD_GoToXY(15,1);
		char buf[2];
		snprintf( buf, 2, "%d", st.logic_level );
		PB_LCD_WriteChar( buf[0] );
}

void printResistance( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 222 );
	
		PB_LCD_GoToXY( 0, 0 );
		char buf[16];
		snprintf( buf, 16, "R: %d%c", st.impedance, 222 );
		PB_LCD_WriteString( buf, 16 );
	
		/* print range */
		PB_LCD_GoToXY(12,1);
		char bufB[5];
		if ( st.logic_level == 0 )
				snprintf( bufB, 5, "  1k" );
		else if ( st.logic_level == 1 )
				snprintf( bufB, 5, " 10k" );
		else if ( st.logic_level == 2 )
				snprintf( bufB, 5, "100k" );
		else
				snprintf( bufB, 5, "    " );
		
		PB_LCD_WriteString( bufB, 4 );
    
    PB_LCD_GoToXY( 0,0 );
}

void printCapacitance( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 'F' );
	
		PB_LCD_GoToXY( 0, 0 );
		char buf[16];
		snprintf( buf, 16, "F: %f%c", capacitanceFromImpedance( st.impedance, st.range ), 'F' );
		PB_LCD_WriteString( buf, 16 );
	
		/* print range */
		PB_LCD_GoToXY(12,1);
		char bufB[5];
		if ( st.logic_level == 0 )
				snprintf( bufB, 5, "  1k" );
		else if ( st.logic_level == 1 )
				snprintf( bufB, 5, " 10k" );
		else if ( st.logic_level == 2 )
				snprintf( bufB, 5, "100k" );
		else
				snprintf( bufB, 5, "    " );
		
		PB_LCD_WriteString( bufB, 4 );
    
    PB_LCD_GoToXY( 0,0 );
}

void printInductance( struct State st )
{
    PB_LCD_GoToXY( 15, 0 );
    PB_LCD_WriteChar( 'F' );
	
		PB_LCD_GoToXY( 0, 0 );
		char buf[16];
		snprintf( buf, 16, "F: %f%c", inductanceFromImpedance( st.impedance, st.range ), 'F' );
		PB_LCD_WriteString( buf, 16 );
	
		/* print range */
		PB_LCD_GoToXY(12,1);
		char bufB[5];
		if ( st.logic_level == 0 )
				snprintf( bufB, 5, "  1k" );
		else if ( st.logic_level == 1 )
				snprintf( bufB, 5, " 10k" );
		else if ( st.logic_level == 2 )
				snprintf( bufB, 5, "100k" );
		else
				snprintf( bufB, 5, "    " );
		
		PB_LCD_WriteString( bufB, 4 );
    
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
