#include "stm32f4xx_hal.h"
#include "button.h"


/* initialize pin PE7 as input */
void Button_init( void )
{
    __HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_InitTypeDef PushButton;
	PushButton.Mode = GPIO_MODE_INPUT;
	PushButton.Pin = GPIO_PIN_7;
	PushButton.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &PushButton);
}

int Button_isPressed( void )
{
    return HAL_GPIO_ReadPin( GPIOE, GPIO_PIN_7 );
}