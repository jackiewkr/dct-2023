#include "stm32f4xx_hal.h"
#include "button.h"

static uint8_t prev_state;

/* initialize pin PE7 as input */
void Button_init( void )
{
    __HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_InitTypeDef PushButton;
	PushButton.Mode = GPIO_MODE_INPUT;
	PushButton.Pin = GPIO_PIN_7;
	PushButton.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &PushButton);
}

/* Check if button is pressed; ignore consecutive presses */
int Button_isPressed( void )
{
    uint8_t state = HAL_GPIO_ReadPin( GPIOE, GPIO_PIN_7 );
    if ( prev_state == state && state == 1 )
    {
        return 0;
    }
    prev_state = state;
    return state;
}