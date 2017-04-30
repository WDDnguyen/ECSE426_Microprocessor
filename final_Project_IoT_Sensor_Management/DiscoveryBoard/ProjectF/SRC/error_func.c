#include "stm32f4xx_it.h"
#include "error_func.h"

// handle errors
void Error_Handler(Error_Code tag)
{
	if (tag == DAC_INIT_ERROR)
	{
		printf("Error: error in initializing DAC");
		while(1);
	}
	else if (tag == DAC_CONFIG_ERROR)
	{
		printf("Error: error in configuring DAC");
		while(1);
	}
	else if (tag == DMA_INIT_ERROR)
	{
		printf("Error: error in initializing DMA");
		while(1);
	}
	else if (tag == DMA_START_ERROR)
	{
		printf("Error: error in starting DMA");
		while(1);
	}
	else if (tag == TIMER_INIT_ERROR)
	{
		printf("Error: error in initializing hardware timer");
		while(1);
	}
	else if (tag == TIMER_CONFIG_ERROR)
	{
		printf("Error: error in configuring hardware timer");
		while(1);
	}
	else if (tag == TIMER_START_ERROR)
	{
		printf("Error: error in starting hardware timer");
		while(1);
	}
	else if (tag == UART_INIT_ERROR)
	{
		printf("Error: error in initializing UART");
		while(1);
	}
	else if (tag == UART_TX_ERROR)
	{
		printf("Error: error in transmitting data via UART");
		while(1);
	}
	else if (tag == UART_RX_ERROR)
	{
		printf("Error: error in receiving data via UART");
		while(1);
	}
	else if (tag == UART_INVALID_LABEL)
	{
		printf("Error: error in axis labeling");
		while(1);
	}
	else if (tag == FLASH_ERASE_ERROR)
	{
		printf("Error: error in erasing flash memory");
		while(1);
	}
	else if (tag == FLASH_ADDR_ERROR)
	{
		printf("Error: error in mapping flash address");
		while(1);
	}
}
