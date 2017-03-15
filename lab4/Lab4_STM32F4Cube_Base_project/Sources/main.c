/*******************************************************************************
  * @file    main.c
  * @author  Ashraf Suyyagh
	* @version V1.2.0
  * @date    17-January-2016
  * @brief   This file demonstrates flasing one LED at an interval of one second
	*          RTX based using CMSIS-RTOS 
  ******************************************************************************
  */

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection
#include "ACCEL_thread.h"
#include "KEYPAD_thread.h"
#include "TEMP_thread.h"
#include "SEG_thread.h"
#include "gpio.h"
#include "keypad.h"
#include "lis3dsh.h"
#include "arm_math.h"

extern osThreadId ACCEL_thread_ID;
extern osThreadId KEYPAD_thread_ID;
extern osThreadId TEMP_thread_ID;
extern osThreadId SEG_thread_ID;

extern TIM_HandleTypeDef HWTimer;
extern TIM_HandleTypeDef TIM2_handle;
extern TIM_HandleTypeDef TIM3_handle;


/**
	These lines are mandatory to make CMSIS-RTOS RTX work with te new Cube HAL
*/
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) { 
  return os_time; 
}
#endif

/**
  * System Clock Configuration
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * Main function
  */
int main (void) {

  osKernelInitialize();                     /* initialize CMSIS-RTOS          */

  HAL_Init();                               /* Initialize the HAL Library     */

  SystemClock_Config();                     /* Configure the System Clock     */
	
	/* User codes goes here*/
  start_ACCEL_thread(NULL);
	start_KEYPAD_thread(NULL);
	start_TEMP_thread(NULL);
	start_SEG_thread(NULL);
	
	//initializeLED_IO();                       /* Initialize LED GPIO Buttons    */
  //start_Thread_LED();                       /* Create LED thread              */
	/* User codes ends here*/
  
	osKernelStart();                          /* start thread execution         */
	osDelay(osWaitForever);
}

void TIM4_IRQHandler(void){
HAL_TIM_IRQHandler(&HWTimer);
}

void TIM2_IRQHandler(void){
HAL_TIM_IRQHandler(&TIM2_handle);
}

void TIM3_IRQHandler(void){
HAL_TIM_IRQHandler(&TIM3_handle);
}

void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(LIS3DSH_SPI_INT1_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == LIS3DSH_SPI_INT1_PIN)
		osSignalSet(ACCEL_thread_ID, 0x00000001);
}

void  HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {	
	if(htim->Instance == TIM4){
		osSignalSet(KEYPAD_thread_ID, 0x00000001);
	}
	else if (htim->Instance == TIM2){
		osSignalSet(TEMP_thread_ID, 0x00000001);
	}
		else if (htim->Instance == TIM3){
		osSignalSet(SEG_thread_ID, 0x00000001);
	} 
}