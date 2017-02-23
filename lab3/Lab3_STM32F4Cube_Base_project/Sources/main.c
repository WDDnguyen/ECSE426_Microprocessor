/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program subroutine
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "lis3dsh.h"


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);


TIM_HandleTypeDef HWTimer;
	
int main(void)
{	
  /* MCU Configuration----------------------------------------------------------*/

  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	
  /* Initialize all configured peripherals */
	
	LIS3DSH_InitTypeDef accelerometer_Init;
	accelerometer_Init.AA_Filter_BW = LIS3DSH_AA_BW_800;
	accelerometer_Init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;
	accelerometer_Init.Axes_Enable = LIS3DSH_XYZ_ENABLE;
	accelerometer_Init.Continous_Update = LIS3DSH_ContinousUpdate_Enabled;
	accelerometer_Init.Full_Scale = LIS3DSH_FULLSCALE_2;
	accelerometer_Init.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	LIS3DSH_Init(&accelerometer_Init);
	
	//Set up pin 
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.Pin = LIS3DSH_SPI_INT1_PIN; // initialize the pins for the LEDs pin
	GPIO_INIT.Mode = GPIO_MODE_IT_FALLING; // Set pins to output push pull mode 
	GPIO_INIT.Pull = GPIO_NOPULL; // No pull for LEDs pins
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM;	//Speed of pin

	HAL_GPIO_Init(GPIOE, &GPIO_INIT);
	
	
	
	// Turn on the interrupt 
	
	LIS3DSH_DRYInterruptConfigTypeDef accelerometer_Interrupt_Init;
	accelerometer_Interrupt_Init.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	accelerometer_Interrupt_Init.Interrupt_signal = LIS3DSH_ACTIVE_LOW_INTERRUPT_SIGNAL;
	accelerometer_Interrupt_Init.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	
	LIS3DSH_DataReadyInterruptConfig(&accelerometer_Interrupt_Init);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn,1,0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	
	//Timer
	__TIM4_CLK_ENABLE();
	
	HWTimer.Instance = TIM4;
	HWTimer.Init.Prescaler = 84000 - 1;
	HWTimer.Init.Period = 0;
	HWTimer.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	HWTimer.Init.RepetitionCounter = 0;
	
	HAL_TIM_Base_Init(&HWTimer);
	HAL_TIM_Base_Start_IT(&HWTimer);



	while (1){
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t Pin){
	if(Pin == LIS3DSH_SPI_INT1_PIN){
		float coordinate[3];
		LIS3DSH_ReadACC(coordinate);
		printf("X value : %f ,",coordinate[0]/9.81);
		printf("Y value : %f ,",coordinate[1]/9.81);
		printf("Z value : %f\n",coordinate[2]/9.81);
	}		
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* Timer){
	if (Timer->Instance == TIM4){
	
	}
}


/** System Clock Configuration*/
void SystemClock_Config(void){

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType 	= RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState 			 	= RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM 				= 8;
  RCC_OscInitStruct.PLL.PLLN 				= 336;
  RCC_OscInitStruct.PLL.PLLP 				= RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ 				= 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};

  RCC_ClkInitStruct.ClockType 			= RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5)!= HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};
	
	/*Configures SysTick to provide 1ms interval interrupts. SysTick is already 
	  configured inside HAL_Init, I don't kow why the CubeMX generates this call again*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* This function sets the source clock for the internal SysTick Timer to be the maximum,
	   in our case, HCLK is now 168MHz*/
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

//Modified
void TIM4_IRQHandler(void){
HAL_TIM_IRQHandler(&HWTimer);
}


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line){
}
#endif

