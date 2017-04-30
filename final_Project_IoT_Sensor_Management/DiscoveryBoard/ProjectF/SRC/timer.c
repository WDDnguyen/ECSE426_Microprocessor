#include "stm32f4xx_it.h"
#include "error_func.h"
#include "timer.h"

TIM_HandleTypeDef timer_dac;
TIM_HandleTypeDef timer_uart;

void TIM_Initialize(void)
{
	TIM_MasterConfigTypeDef timer_config;
  
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	
  // config dac timer
  timer_dac.Instance = DAC_TIMER;  
  timer_dac.Init.Period = PERIOD_DAC;          
  timer_dac.Init.Prescaler = PRESCALE_DAC;       
  timer_dac.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;    
  timer_dac.Init.CounterMode = TIM_COUNTERMODE_UP; 
  if (HAL_TIM_Base_Init(&timer_dac) != HAL_OK)
	{
		Error_Handler(TIMER_INIT_ERROR);
	}

  // config trigger
  timer_config.MasterOutputTrigger = TIM_TRGO_UPDATE;
  timer_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
  if (HAL_TIMEx_MasterConfigSynchronization(&timer_dac, &timer_config) != HAL_OK)
	{
		Error_Handler(TIMER_CONFIG_ERROR);
	}
  
	// dac timer start
  if (HAL_TIM_Base_Start_IT(&timer_dac) != HAL_OK)
	{
		Error_Handler(TIMER_START_ERROR);
	}
	
	// config uart timer
	timer_uart.Instance = UART_TIMER;
	timer_uart.Init.Prescaler = PERIOD_UART;
	timer_uart.Init.Period = PRESCALE_UART;
	timer_uart.Init.CounterMode = TIM_COUNTERMODE_UP;
	timer_uart.Init.ClockDivision = 0;
	if (HAL_TIM_Base_Init(&timer_uart) != HAL_OK)
	{
		Error_Handler(TIMER_INIT_ERROR);
	}
	
	// uart timer start
	if (HAL_TIM_Base_Start_IT(&timer_uart) != HAL_OK)
	{
		Error_Handler(TIMER_START_ERROR);
	}
	HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}
