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
#include "core_cm4.h"
#include <stdio.h>


#include <math.h>

/* Private variables ---------------------------------------------------------*/

typedef struct FIR_coeff{
float coeffArray[5];
}FIR_coeff;


int FIR_C(float* inputArray, float* OutputArray, FIR_coeff* coeff, int Length, int Order);

void initializeAccelerometer();
void initializeAccelerometerPin();
void initializeAccelerometerInterrupt();
void initializeTimer();


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);


TIM_HandleTypeDef HWTimer;

float xArray[5];
float yArray[5];
float zArray[5];

float xOutputArray[1];
float yOutputArray[1];
float zOutputArray[1];

int Length = 5;
int Order = 4;
int invalidCount = 4;

FIR_coeff coeff;	

float gravity = 9.81;


int interruptCounter = 0;

int main(void)
{	
  /* MCU Configuration----------------------------------------------------------*/

  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	
	//Set up accelerometer 
	initializeAccelerometer();	
	
	//Set up  accelerometer pin 
	initializeAccelerometerPin();
		
	// initialize interrupt for accelerometer 
	initializeAccelerometerInterrupt();
	
	//Timer
	initializeTimer();
	
	// Set up Moving average filter 
	
	
	// Initializing coefficients,Order,Length,Arrays for FIR filter
	
	coeff.coeffArray[0] = 0.1;
	coeff.coeffArray[1] = 0.15;
	coeff.coeffArray[2] = 0.5;
	coeff.coeffArray[3] = 0.15;
	coeff.coeffArray[4] = 0.1;

	while (1){
	}
}


void initializeAccelerometer(){
 	
	LIS3DSH_InitTypeDef accelerometer_Init;
	accelerometer_Init.AA_Filter_BW = LIS3DSH_AA_BW_800;
	accelerometer_Init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;
	accelerometer_Init.Axes_Enable = LIS3DSH_XYZ_ENABLE;
	accelerometer_Init.Continous_Update = LIS3DSH_ContinousUpdate_Enabled;
	accelerometer_Init.Full_Scale = LIS3DSH_FULLSCALE_2;
	accelerometer_Init.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	LIS3DSH_Init(&accelerometer_Init);
}

void initializeAccelerometerPin(){

	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.Pin = LIS3DSH_SPI_INT1_PIN; 
	GPIO_INIT.Mode = GPIO_MODE_IT_FALLING;  
	//GPIO_INIT.Pull = GPIO_NOPULL; 
	GPIO_INIT.Pull = GPIO_PULLDOWN;
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOE, &GPIO_INIT);
	
}

void initializeAccelerometerInterrupt(){
	
	LIS3DSH_DRYInterruptConfigTypeDef accelerometer_Interrupt_Init;
	accelerometer_Interrupt_Init.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	accelerometer_Interrupt_Init.Interrupt_signal = LIS3DSH_ACTIVE_LOW_INTERRUPT_SIGNAL;
	accelerometer_Interrupt_Init.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	
	LIS3DSH_DataReadyInterruptConfig(&accelerometer_Interrupt_Init);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn,1,0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	
}

void initializeTimer(){

	__TIM4_CLK_ENABLE();
	
	HWTimer.Instance = TIM4;
	HWTimer.Init.Prescaler = 84000 - 1;
	HWTimer.Init.Period = 0;
	HWTimer.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	HWTimer.Init.RepetitionCounter = 0;
	
	HAL_TIM_Base_Init(&HWTimer);
	HAL_TIM_Base_Start_IT(&HWTimer);

}


void HAL_GPIO_EXTI_Callback(uint16_t Pin){
	if(Pin == LIS3DSH_SPI_INT1_PIN){
		float coordinate[3];
		
		LIS3DSH_ReadACC(coordinate);
	
		int xFIROutput;
		int yFIROutput;
		int zFIROutput;
		
		/*if (interruptCounter++ == 5000){
		interruptCounter = 0;
		printf("X value : %f ,",coordinate[0]/gravity);
		printf("Y value : %f ,",coordinate[1]/gravity);
		printf("Z value : %f\n",coordinate[2]/gravity);
		}*/
		
		if (invalidCount == 0){
			
		
		for (int i = 5; i < 5; i++){ 
		xArray[5-i] = xArray[4-i];
		yArray[5-i] = yArray[4-i];
		zArray[5-i] = zArray[4-i];	
		}
		
		xArray[0] = coordinate[0];
		yArray[0] = coordinate[1];
		zArray[0] = coordinate[2];
		
		xFIROutput = FIR_C(xArray, xOutputArray, &coeff, Length, Order);
		yFIROutput = FIR_C(yArray, yOutputArray, &coeff, Length, Order);
		zFIROutput = FIR_C(zArray, zOutputArray, &coeff, Length, Order);
		
		
		printf("X value : %f ,",xOutputArray[0]/gravity);
		printf("Y value : %f ,",yOutputArray[0]/gravity);
		printf("Z value : %f\n",zOutputArray[0]/gravity);
	
}
		
		
		// If there is less than 4 input at the start then add into the array and wait till there is 5 value 
		else {
			xArray[invalidCount] = coordinate[0];
			yArray[invalidCount] = coordinate[1];
			zArray[invalidCount] = coordinate[2];
			invalidCount -= 1;
			printf("FIRST 4 POLL DOESNT GIVE VALUE : %d \n", invalidCount);
	}
		
	}
	
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* Timer){
	if (Timer->Instance == TIM4){
	
	}
}

int FIR_C(float* inputArray, float* OutputArray, FIR_coeff* coeff, int Length, int Order){
	int i,j,k;
        float y;

        for (j = 0 ; j < Length - Order; j++){
							OutputArray[j] = inputArray[0] * coeff->coeffArray[0] + inputArray[1] * coeff->coeffArray[1] + inputArray[2] * coeff->coeffArray[2] + inputArray[3] * coeff->coeffArray[3] + inputArray[4] * coeff->coeffArray[4];
        }
        return 0;
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

