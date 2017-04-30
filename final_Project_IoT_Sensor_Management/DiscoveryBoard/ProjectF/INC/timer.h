#ifndef __TIMER
#define __TIMER

#define DAC_TIMER TIM2
#define UART_TIMER TIM3

#define PERIOD_DAC 2000-1
#define PRESCALE_DAC 420-1
#define PERIOD_UART 20900-1
#define PRESCALE_UART 42000-1

/**
  * @brief  Initialize timer(s)
  * @param  None
	* @retval None
  */
void TIM_Initialize(void);

#endif
