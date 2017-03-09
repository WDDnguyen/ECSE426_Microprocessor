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
#include "KeyPad.h"

#include "arm_math.h"
#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Private variables ---------------------------------------------------------*/
void initializeAccelerometer();
void initializeAccelerometerPin();
void initializeAccelerometerInterrupt();
void initializeTimer();
void controlRollLED();
void controlPitchLED();
void keyPadInput();

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);

//Initialize TIM4 timer 
TIM_HandleTypeDef HWTimer;

//FIR filter variables
uint32_t blockSize = 1;
float32_t firStateF32[10 + 5 - 1];
float32_t firStateF32y[10 + 5 - 1];
float32_t firStateF32z[10 + 5 - 1];

float32_t coefficients[5] = {0.1,0.15,0.5,0.15,0.1};
float32_t coefficientsy[5] = {0.1,0.15,0.5,0.15,0.1};
float32_t coefficientsz[5] = {0.1,0.15,0.5,0.15,0.1};

arm_fir_instance_f32 s = {5, firStateF32,coefficients};
arm_fir_instance_f32 sy = {5, firStateF32y,coefficientsy};
arm_fir_instance_f32 sz = {5, firStateF32z,coefficientsz};

int invalidCount = 4;

//Use to slow down prints  
int interruptCounter = 0;

// roll and pitch variable initialization
int multiplier  = 100 ;
int current = 0;

int roll = 0;
int pitch = 0;

int position = 1;
// state 0 -> roll Input , state 1 -> pitch input 
int state = 0;

//Values to be used in GPIO.C for LEDs intensity controlling 
volatile int rollValue = 0;
volatile int pitchValue = 0;
volatile int keypadValuesSet = 0;

//variables for accelerometer
float32_t xCalibrated = 0;
float32_t yCalibrated = 0;
float32_t zCalibrated = 0;

float32_t xArray[5];
float32_t yArray[5];
float32_t zArray[5];

float32_t xOutputArray[1];
float32_t yOutputArray[1];
float32_t zOutputArray[1];

float32_t gravity = 9.81;

//variable for numpad
int pressedCounter = 0;
int pressedValue = -5;

int main(void)
{	
  /* MCU Configuration----------------------------------------------------------*/

  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	
	//initialize filters for x,y,z
	arm_fir_init_f32(&s, 5,(float32_t *)&coefficients[0],&firStateF32[0],blockSize);
	arm_fir_init_f32(&sy, 5,(float32_t *)&coefficientsy[0],&firStateF32y[0],blockSize);
	arm_fir_init_f32(&sz, 5,(float32_t *)&coefficientsz[0],&firStateF32z[0],blockSize);
	
	//Set up accelerometer 
	initializeAccelerometer();	
	
	//Set up  accelerometer pin 
	initializeAccelerometerPin();
		
	// initialize interrupt for accelerometer 
	initializeAccelerometerInterrupt();

	//Enable interrupt and set priority for accelerometer 
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);	
	
	//initialize Timer on Tim4
	initializeTimer();

  //Enable Interrupt and set priority for Timer
	HAL_NVIC_SetPriority(TIM4_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);	
	
	//initialize KeyPad
	initializeKeyPad();

	// initialize GPIOs
	GPIO_Init();
	
	while (1){
	}
}

//function to set up roll and pitch value  after input on KeyPad
void setUpRollValue(int roll){
rollValue = roll;
}

void setUpPitchValue(int pitch){
pitchValue = pitch;
}

void initializeAccelerometer(){
	//initializer for accelerometer
	LIS3DSH_InitTypeDef accelerometer_Init;
	// filter to use if frequency of signal is too high. 
	accelerometer_Init.AA_Filter_BW = LIS3DSH_AA_BW_800;
	//sample 25 times per second
	accelerometer_Init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;
	//enable X,Y,Z coordinates
	accelerometer_Init.Axes_Enable = LIS3DSH_XYZ_ENABLE;
	//Continously sample beside of blocking 
	accelerometer_Init.Continous_Update = LIS3DSH_ContinousUpdate_Enabled;
	//set maximum scale to +/- 2g.
	accelerometer_Init.Full_Scale = LIS3DSH_FULLSCALE_2;
	accelerometer_Init.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	
	//Set up the accelerometer with the configuration
	LIS3DSH_Init(&accelerometer_Init);
}

void initializeAccelerometerPin(){

	//set up  clock for port E 
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	// initialize PIN setup
	GPIO_InitTypeDef GPIO_INIT;
	// set pin to specific accelerometer pin
	GPIO_INIT.Pin = LIS3DSH_SPI_INT1_PIN; 
	// trigger when falling edge of samp;ing
	GPIO_INIT.Mode = GPIO_MODE_IT_FALLING;
	
	GPIO_INIT.Pull = GPIO_PULLDOWN;
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	//set up pin on PORT E
	HAL_GPIO_Init(GPIOE, &GPIO_INIT);
	
}

void initializeAccelerometerInterrupt(){
	
	//initialize configuration set up for Interrupt
	LIS3DSH_DRYInterruptConfigTypeDef accelerometer_Interrupt_Init;
	//when to trigger the interrupt 
	accelerometer_Interrupt_Init.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	
	//Active low when getting signal 
	accelerometer_Interrupt_Init.Interrupt_signal = LIS3DSH_ACTIVE_LOW_INTERRUPT_SIGNAL;
	accelerometer_Interrupt_Init.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	
	LIS3DSH_DataReadyInterruptConfig(&accelerometer_Interrupt_Init);
		
}

void initializeTimer(){

	__TIM4_CLK_ENABLE();
	
	//set timer as TIM 4
	HWTimer.Instance = TIM4;
	
	//Frequency wanted = frequency of TIM4 / (Prescaler + Period) = 2kHz
	HWTimer.Init.Prescaler = 420 - 1;
	//counter
	HWTimer.Init.Period = 100;
	//count upward 
	HWTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	HWTimer.Init.RepetitionCounter = 0;

//Setting up PWM	
	TIM_OC_InitTypeDef outputChannelInit;

	outputChannelInit.OCMode = TIM_OCMODE_PWM1;
	outputChannelInit.Pulse = 100;
	outputChannelInit.OCFastMode = TIM_OCFAST_ENABLE;
	outputChannelInit.OCPolarity = TIM_OCPOLARITY_HIGH;

	HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_4);

	//Start timer 
	HAL_TIM_Base_Init(&HWTimer);
	HAL_TIM_Base_Start_IT(&HWTimer);
	
	//START PWM 
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_4);
}

void HAL_GPIO_EXTI_Callback(uint16_t Pin){
	if(Pin == LIS3DSH_SPI_INT1_PIN){
		
	float32_t coordinate[3];
	LIS3DSH_ReadACC(coordinate);
	
	xCalibrated = coordinate[0]* 0.846 + 0.372;
	yCalibrated = coordinate[1]* 0.846	+ 0.38;
	zCalibrated = coordinate[2] * 0.846 + 0.38;			
		
	//printf("X value : %f ,",coordinate[0]/gravity);
	//printf("Y value : %f ,",coordinate[1]/gravity);
	//printf("Z value : %f\n",coordinate[2]/gravity);
	
	//printf("NOT FILTERED: %f,", xCalibrated/gravity);
	//printf("%f,", yCalibrated/gravity);
	//printf("%f\n", zCalibrated/gravity);
		
		
		
	if (invalidCount == 0){
			
		for (int i = 5; i < 5; i++){ 
			xArray[5-i] = xArray[4-i];
			yArray[5-i] = yArray[4-i];
			zArray[5-i] = zArray[4-i];
		}
		
		xArray[0] = xCalibrated;
		yArray[0] = yCalibrated;
		zArray[0] = zCalibrated;
		
		//filtering the calibrated value using DSP filter
		arm_fir_f32(&s, &xArray[0], &xOutputArray[0],blockSize);
		arm_fir_f32(&sy, &yArray[0], &yOutputArray[0],blockSize);
		arm_fir_f32(&sz, &zArray[0], &zOutputArray[0],blockSize);	
		
		//printf("FILTERED : %f,%f,%f\n",(xOutputArray[0]/gravity),(yOutputArray[0]/gravity), (zOutputArray[0]/gravity));
		//printf("X value : %f ,",(xOutputArray[0]/gravity));
		//printf("Y value : %f \n",(yOutputArray[0]/gravity));	
	}
		
	else {
		xArray[invalidCount] = coordinate[0];
		yArray[invalidCount] = coordinate[1];
		zArray[invalidCount] = coordinate[2];
		invalidCount -= 1;
		}	
	}
}

//Exception Handling for timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *Timer){
	
	
	if (Timer->Instance == TIM4){
		int input = KeyPadGetValue();
		
    keyPadInput(input);		

		if(rollValue > 0) {
			controlRollLED();
		}
		 if (pitchValue > 0 ){
			controlPitchLED();
		}
		
	}
	  
		}

void keyPadInput(int input){
if(input != -1 && pressedValue < 0){
			pressedValue = input;
			
		}
		
		if(input == pressedValue){
			if (pressedCounter == 500){
								
				displayValue(pressedValue,position);
				position = position + 1;

				if (state == 1 && pressedValue != Key_hash && pressedValue != Key_star ){
					current = pitch;
					pitch = 10*pitch + pressedValue;
					if (pitch > 180){
					pitch =0;
					displayValue(4,4);
					}
					
					if (position > 3){
					position = 1;
					}		
				}
				else if ( state == 1 && pressedValue == Key_hash){
					
					state = 0;
					setUpPitchValue(pitch);
					displayValue(5,4);
					pitch = 0; 
					position = 1;
				}
				else if ( state == 1 && pressedValue == Key_star){
							pitch = current;
							position -= 2;
							displayValue(6,4);
					if(position <=0){
					position = 1;
					current = 0;
					pitch = 0;
					state = 0;
					}
				}
				
				else if (state == 0 && pressedValue != Key_hash && pressedValue != Key_star){
					current = roll;
					roll = 10*roll + pressedValue;
					if (roll > 180){
						roll =0;
						displayValue(1,4);				
					}				
				}
				else if ( state == 0 && pressedValue == Key_hash ){
					state = 1;
					setUpRollValue(roll);
					displayValue(2,4);
					roll = 0; 
					position = 1;
				}
				
				else if ( state == 0 && pressedValue == Key_star){
					roll = current;
					position -= 2;
					displayValue(3,4);
					if(position <=0){
					position = 1;
					current = 0;
					roll = 0;
					}
				}
										
				pressedCounter = 0;
				pressedValue = -1;		
						
				
			}else{
				pressedCounter++;
			}
		}//RESET COUNTER
		else if (input != -1){ 
			pressedCounter = 0;
			pressedValue = -1;
			
		}
		

}
void controlRollLED(){
float32_t convertedRollValue = rollValue - 90;
			float32_t xAccelerometerAngle = xOutputArray[0]/gravity;
			float32_t rollDifference = fabs(xAccelerometerAngle - convertedRollValue);
			
			if (rollDifference < 5){
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_1, 0);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_3, 0);
			
			}
			else {
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_1, rollDifference);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_3, rollDifference);
			}

}

void controlPitchLED(){
	
			float32_t convertPitchValue = pitchValue - 90;
			float32_t yAccelerometerAngle = yOutputArray[0]/gravity;
			float32_t pitchDifference = fabs(yAccelerometerAngle - convertPitchValue);
			
			 if(pitchDifference < 5){
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_2, 0);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_4, 0);
			}
			else {
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_2, pitchDifference);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_4, pitchDifference);
			
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

//Modified the handler for TIM4  
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

