#ifndef __GPIO
#define __GPIO

#define GPIO_PORT_ACC GPIOE
#define GPIO_PORT_BUTTON GPIOA
#define GPIO_PORT_DAC GPIOA
#define GPIO_PORT_UART GPIOA
#define GPIO_PORT_LED GPIOD

#define GPIO_ACC_IT GPIO_PIN_0

#define GPIO_BUTTON_IT GPIO_PIN_0

#define GPIO_UART_TX GPIO_PIN_2
#define GPIO_UART_RX GPIO_PIN_3

#define GPIO_DAC_CH1 GPIO_PIN_4
#define GPIO_DAC_CH2 GPIO_PIN_5

#define LED_L GPIO_PIN_12
#define LED_T	GPIO_PIN_13
#define LED_R GPIO_PIN_14
#define LED_B GPIO_PIN_15

/**
  * @brief  GPIO Initialization
  * @param  None
	* @retval None
  */
void GPIO_Initialize(void);

#endif
