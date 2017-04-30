#include "stm32f4xx_it.h"
#include "error_func.h"
#include "gpio.h"

void GPIO_Initialize(void)
{
	GPIO_InitTypeDef gpio_adc_ch1;
	GPIO_InitTypeDef gpio_acc_irr;
	GPIO_InitTypeDef gpio_button;
	GPIO_InitTypeDef gpio_uart_tx;
	GPIO_InitTypeDef gpio_uart_rx;
	GPIO_InitTypeDef gpio_led;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	// dac
	gpio_adc_ch1.Pin = GPIO_DAC_CH1;
  gpio_adc_ch1.Mode = GPIO_MODE_ANALOG;
  gpio_adc_ch1.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIO_PORT_DAC, &gpio_adc_ch1);
	
	// acccelerometer IT
	gpio_acc_irr.Pin = GPIO_ACC_IT;
	gpio_acc_irr.Mode = GPIO_MODE_IT_RISING; 
	gpio_acc_irr.Pull = GPIO_PULLDOWN;
	gpio_acc_irr.Speed =  GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIO_PORT_ACC, &gpio_acc_irr);
	
	// LED
	gpio_led.Pin = LED_L | LED_T | LED_R | LED_B;
	gpio_led.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_led.Pull = GPIO_NOPULL;
	gpio_led.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIO_PORT_LED, &gpio_led);
	
	// button
	gpio_button.Pin = GPIO_BUTTON_IT;
	gpio_button.Mode = GPIO_MODE_IT_RISING;
	gpio_button.Pull = GPIO_NOPULL;
	gpio_button.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIO_PORT_BUTTON, &gpio_button);
	
	// uart
	gpio_uart_tx.Pin = GPIO_UART_TX;
	gpio_uart_tx.Mode = GPIO_MODE_AF_PP;
	gpio_uart_tx.Pull = GPIO_NOPULL;
	gpio_uart_tx.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_uart_tx.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIO_PORT_UART, &gpio_uart_tx);
	
	gpio_uart_rx.Pin = GPIO_UART_RX;
	gpio_uart_rx.Mode = GPIO_MODE_AF_PP;
	gpio_uart_rx.Pull = GPIO_NOPULL;
	gpio_uart_rx.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_uart_rx.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIO_PORT_UART, &gpio_uart_rx);
	
	// set up external interrupt
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0,1);
}
