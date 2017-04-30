#include "stm32f4xx_it.h"
#include "error_func.h"
#include "uart.h"

uint8_t tx_x_u[BUFFER_SIZE];
uint8_t tx_x_l[BUFFER_SIZE];
uint8_t tx_y_u[BUFFER_SIZE];
uint8_t tx_y_l[BUFFER_SIZE];
uint8_t tx_z_u[BUFFER_SIZE];
uint8_t tx_z_l[BUFFER_SIZE];
uint8_t rx_pitch[BUFFER_SIZE];
int rx_pitch_cvt[BUFFER_SIZE];
uint8_t rx_roll[BUFFER_SIZE];

UART_HandleTypeDef uart_handle;

void UART_Initialize(void)
{
	UART_InitTypeDef uart_init;
	
	__HAL_RCC_USART2_CLK_ENABLE();
	
	// Initialize TX
	uart_init.BaudRate = 115200;
	uart_init.WordLength = UART_WORDLENGTH_8B;
	uart_init.StopBits = UART_STOPBITS_1;
	uart_init.Parity = UART_PARITY_NONE;
	uart_init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_init.Mode = UART_MODE_TX_RX;
	uart_init.OverSampling = UART_OVERSAMPLING_16;
	
	uart_handle.Init = uart_init;
	uart_handle.Instance = USART2;
	
	if (HAL_UART_Init(&uart_handle) != HAL_OK)
	{
		Error_Handler(UART_INIT_ERROR);
	}
	
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

// transmit one-shot acc data
void UART_Transmit(float* data, Acc_Label label)
{	
	// data formatting
	dataFormat(data, label);
	
	// data transmission
	if (label == x_label)
	{
		if(HAL_UART_Transmit(&uart_handle, tx_x_u, BUFFER_SIZE, TIMEOUT) != HAL_OK)
		{
			Error_Handler(UART_TX_ERROR);
		}
		if(HAL_UART_Transmit(&uart_handle, tx_x_l, BUFFER_SIZE, TIMEOUT) != HAL_OK)
		{
			Error_Handler(UART_TX_ERROR);
		}
	}
	else if (label == y_label)
	{
		if(HAL_UART_Transmit(&uart_handle, tx_y_u, BUFFER_SIZE, TIMEOUT) != HAL_OK)
		{
			Error_Handler(UART_TX_ERROR);
		}
		if(HAL_UART_Transmit(&uart_handle, tx_y_l, BUFFER_SIZE, TIMEOUT) != HAL_OK)
		{
			Error_Handler(UART_TX_ERROR);
		}
	}
	else if (label == z_label)
	{
		if(HAL_UART_Transmit(&uart_handle, tx_z_u, BUFFER_SIZE, TIMEOUT) != HAL_OK)
		{
			Error_Handler(UART_TX_ERROR);
		}
		if(HAL_UART_Transmit(&uart_handle, tx_z_l, BUFFER_SIZE, TIMEOUT) != HAL_OK)
		{
			Error_Handler(UART_TX_ERROR);
		}
	}
	else
	{
		Error_Handler(UART_INVALID_LABEL);
	}
}

// receive one-shot acc data
void UART_Receive(void)
{	
	int i;
	
	// data receive
	while (HAL_UART_Receive(&uart_handle, rx_pitch, BUFFER_SIZE, TIMEOUT) != HAL_OK)
	{
//		Error_Handler(UART_RX_ERROR);
	}
	
	// data format
	for (i=0; i<BUFFER_SIZE; i++)
	{
		if ((rx_pitch[i] & 0x80) != 0x00)
		{
			rx_pitch[i] = (~rx_pitch[i]+0x01);
			rx_pitch_cvt[i] = -rx_pitch[i];
		}
		else
		{
			rx_pitch_cvt[i] = rx_pitch[i];
		}
		rx_pitch_cvt[i] = rx_pitch_cvt[i] + 90;
	}
}

// transmit acc data
void UART_Transmit_Arr(float* acc_x, float* acc_y, float* acc_z)
{
	UART_Transmit(acc_x, x_label);
	HAL_Delay(50);
	UART_Transmit(acc_y, y_label);
	HAL_Delay(50);
	UART_Transmit(acc_z, z_label);
}

// get pitch data
int data_pitch(int index)
{
	return rx_pitch_cvt[index];
}

// get roll data
int data_roll(int index)
{
	return rx_roll[index];
}

// format data
void dataFormat(float* data, Acc_Label label)
{
	int i;
	uint16_t tbc[BUFFER_SIZE];

	for (i=0; i<BUFFER_SIZE; i++)
	{
		// compute 2's complement
		if (data[i] < 0)
		{
			tbc[i] = complConvert(data[i]);
		}
		else
		{
			tbc[i] = (uint16_t)data[i];
		}
		
		// identify corresponding buffer to update
		if (label == x_label)
		{
			tx_x_u[i] = data16to8_R(tbc[i]);
			tx_x_l[i] = data16to8_L(tbc[i]);
		}
		else if (label == y_label)
		{
			tx_y_u[i] = data16to8_R(tbc[i]);
			tx_y_l[i] = data16to8_L(tbc[i]);
		}
		else if (label == z_label)
		{
			tx_z_u[i] = data16to8_R(tbc[i]);
			tx_z_l[i] = data16to8_L(tbc[i]);
		}
		else
		{
			Error_Handler(UART_INVALID_LABEL);
		}
	}	
}

// 16 bits to 8 bits (MS part)
uint8_t data16to8_R(uint16_t data)
{
	return (uint8_t)(data >> 8);
}

// 16 bits to 8 bits (LS part)
uint8_t data16to8_L(uint16_t data)
{
	return (uint8_t)data;
}

// compute 2's complement
uint16_t complConvert(float value)
{
	uint16_t cvt = (uint16_t)(-value);
	cvt = ~cvt + 0x0001;
	return cvt;
}
