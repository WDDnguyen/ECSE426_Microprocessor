/**
  ******************************************************************************
  * @title   ECSE 426 Final Project ./SRC/main.c
  * @author  Group 2
  * @version V1.2
  * @date    8-April-2017
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "timer.h"
#include "dac.h"
#include "gpio.h"
#include "accelerometer.h"
#include "uart.h"
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct{
	float acc_x[BUFFER_SIZE];
	float acc_y[BUFFER_SIZE];
	float acc_z[BUFFER_SIZE];
}ACC_BUFFER;
/* Private define ------------------------------------------------------------*/
#define FLASH_CLEAR_INIT 		0x08000100
#define FLASH_CLEAR_END 		0X080FFFF0
/* Private variables ---------------------------------------------------------*/
ACC_Angle angle_rx;
ACC_BUFFER buffer;
volatile Flag_Sig acc_flag;
volatile Flag_Sig button_flag;
volatile Flag_Sig tx_enable;
volatile int tx_count;
/* Private function prototypes -----------------------------------------------*/

static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	// local variables
	int i;
	int acc_count = 0;
	tx_count = 0;
	button_flag = OFF;
	
	// buffer initialization
	for (i=1; i<BUFFER_SIZE; i++)
	{
		buffer.acc_x[i] = 0;
		buffer.acc_y[i] = 0;
		buffer.acc_z[i] = 0;
	}
	
	// global initialization
  HAL_Init();
  
  // system clock configuration
  SystemClock_Config();
	
	// peripheral initialization
  TIM_Initialize();
	DAC_Initialize();
	DMA_Initialize();
	UART_Initialize();
	GPIO_Initialize();
//	Flash_Erase_Init((uint32_t)FLASH_CLEAR_INIT, (uint32_t)FLASH_CLEAR_END);
	
  // program start
  while (1)
  {
		// Clear flash memory
		/*
		if (button_flag == OFF)
		{
			Flash_Erase();
			DCache_Flush();
			ICache_Flush();
		}
		*/
		
		// button detection
		if (button_flag == ON)
		{
			// check accelerometer flag
			if (acc_flag == ON)
			{
				acc_flag = OFF;
				
				// compute accelerations
				cmpAngle();
				angle_rx = getAngle();
				if (acc_count < BUFFER_SIZE)
				{
					buffer.acc_x[acc_count] = getACC().x_val;
					buffer.acc_y[acc_count] = getACC().y_val;
					buffer.acc_z[acc_count] = getACC().z_val;
					acc_count++;
				}
			}
			
			// TX (send only once)
			if (tx_enable == ON && acc_count == BUFFER_SIZE)
			{
				tx_enable = OFF;
				// turn off TX, turn on RX
				tx_count = 1;
				HAL_GPIO_WritePin(GPIO_PORT_LED, LED_T, GPIO_PIN_SET);
				// transmit
				UART_Transmit_Arr(buffer.acc_x, buffer.acc_y, buffer.acc_z);
				HAL_GPIO_WritePin(GPIO_PORT_LED, LED_T, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIO_PORT_LED, LED_B, GPIO_PIN_SET);
			}

			// RX (uncomment this part when ready to receive)
			
			if (tx_count == 1)
			{
				// receive data
				UART_Receive();
				HAL_GPIO_WritePin(GPIO_PORT_LED, LED_L, GPIO_PIN_SET);
				tx_count++;
			}
			else
			{
				for (i=0; i<BUFFER_SIZE; i++)
				{
					// update pitch & roll angle
					angle_rx.pitch = data_pitch(i);
//					angle_rx.roll = data_roll(i);
					// set DAC
					DAC_Set(angle_rx.pitch);
				}
			}
						
		}
  }
}

/**
  * @brief  System Clock Configuration
  *            SYSCLK(Hz)                     = 168000000
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

	/* Set time between system timer interrupt to 1ms */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	
	/* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
