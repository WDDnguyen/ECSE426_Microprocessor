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


#include <math.h>

/* Private variables ---------------------------------------------------------*/
void initializeAccelerometer();
void initializeAccelerometerPin();
void initializeAccelerometerInterrupt();
void initializeTimer();


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);


TIM_HandleTypeDef HWTimer;

//FIR filter variables
uint32_t blockSize = 1;
float32_t firStateF32[10 + 5 - 1];
float32_t firStateF32y[10 + 5 - 1];
float32_t coefficients[5] = {0.1,0.15,0.5,0.15,0.1};
float32_t coefficientsy[5] = {0.1,0.15,0.5,0.15,0.1};
int invalidCount = 4;
arm_fir_instance_f32 s = {5, firStateF32,coefficients};
arm_fir_instance_f32 sy = {5, firStateF32y,coefficientsy};

//variables for fil 
int interruptCounter = 0;

// roll and pitch variable initialization
int multiplier  = 100 ;
int current = 0;

int roll = 0;
int pitch = 0;

int position = 3;
int state = 0;

//Values to be used in GPIO.C for LEDs intensity controlling 
volatile int rollValue = 0;
volatile int pitchValue = 0;
volatile int keypadValuesSet = 0;


//variables for accelerometer
float32_t xCalibrated = 0;
float32_t yCalibrated = 0;

float32_t xArray[5];
float32_t yArray[5];

float32_t xOutputArray[1];
float32_t yOutputArray[1];

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
	
	arm_fir_init_f32(&s, 5,(float32_t *)&coefficients[0],&firStateF32[0],blockSize);
	arm_fir_init_f32(&sy, 5,(float32_t *)&coefficientsy[0],&firStateF32y[0],blockSize);
	
	//Set up accelerometer 
	initializeAccelerometer();	
	
	//Set up  accelerometer pin 
	initializeAccelerometerPin();
		
	// initialize interrupt for accelerometer 
	initializeAccelerometerInterrupt();

	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);	
	
	//initialize Timer
	initializeTimer();

	HAL_NVIC_SetPriority(TIM4_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);	
	
	//initialize KeyPad
	initializeKeyPad();

	// initialize GPIOs
	GPIO_Init();
	
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
		
}

void initializeTimer(){

	__TIM4_CLK_ENABLE();
	
	HWTimer.Instance = TIM4;
	HWTimer.Init.Prescaler = 42000 - 1;
	HWTimer.Init.Period = 1;
	HWTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	HWTimer.Init.RepetitionCounter = 0;
	
	HAL_TIM_Base_Init(&HWTimer);
	HAL_TIM_Base_Start_IT(&HWTimer);
	
}

void HAL_GPIO_EXTI_Callback(uint16_t Pin){
	if(Pin == LIS3DSH_SPI_INT1_PIN){
		
		float32_t coordinate[3];
		
		LIS3DSH_ReadACC(coordinate);
	
		
		 xCalibrated = coordinate[0]* 0.846 + 0.372;
		 yCalibrated = coordinate[1]* 0.846	+ 0.38;
		
		
		/*
		if (interruptCounter++ == 5000){
		interruptCounter = 0;
		printf("X value : %f ,",coordinate[0]/gravity);
		printf("Y value : %f ,",coordinate[1]/gravity);
		printf("Z value : %f\n",coordinate[2]/gravity);
		
		printf("calibrated X value : %f , ", xCalibrated/gravity);
		printf("calibrated X value : %f\n\n", yCalibrated/gravity);
		
		}
		*/
		
		if (invalidCount == 0){
			
		for (int i = 5; i < 5; i++){ 
		xArray[5-i] = xArray[4-i];
		yArray[5-i] = yArray[4-i];
		}
		
		xArray[0] = xCalibrated;
		yArray[0] = yCalibrated;
		
		arm_fir_f32(&s, &xArray[0], &xOutputArray[0],blockSize);
		arm_fir_f32(&sy, &yArray[0], &yOutputArray[0],blockSize);
			
		//printf("X value : %f ,",(xOutputArray[0]/gravity));
		//printf("Y value : %f \n",(yOutputArray[0]/gravity));
		
}
		
		// If there is less than 4 input at the start then add into the array and wait till there is 5 value 
		else {
			xArray[invalidCount] = coordinate[0];
			yArray[invalidCount] = coordinate[1];
			invalidCount -= 1;
			//printf("FIRST 4 POLL DOESNT GIVE VALUE : %d \n", invalidCount);
	}
			
	}
	
}

void setRollInput(int roll){
	printf("SET VALUE FOR ROLL INPUT IS : %d\n", roll);
	rollValue = roll;
}

void setPitchInput(int pitch){
	printf("SET VALUE FOR PITCH INPUT IS : %d\n", pitch);
	pitchValue = pitch;
}	

void rollInput(int input){
// modify back to hash
	if (input == Key_hash){
		if (roll > 180){
		printf("Retype roll value : %d \n", roll);
		state = 0;
			
		}
		else {
		printf("Roll value is : %d\n", roll);
		
		state = 1; 
		setRollInput(roll);
		//rollLED(roll); // test
		displayValue(5,4);
		}
		position = 3;
		multiplier = 1;
		roll = 0;
		
	}
	
	else if (input == Key_star){
		printf("Deleted current value\n");
		roll = roll - current;
		multiplier = multiplier / 10;
		position++;
		displayValue(-1,position);
	}
	else{
		printf("Value Of KeyPad is : %d\n", input);
		roll = roll + input * multiplier;
		multiplier = multiplier * 10;
		current = input * multiplier;
	
		displayValue(input,position);
		position--;
	}
}

void pitchInput(int input){
			// modify back to hash
			if (input == Key_hash){
				
				if (pitch > 180){
					printf("Retype pitch value : %d \n", pitch);
					resetDisplay();
					state = 1;
					
				}
				else {
				printf("Pitch value is : %d\n", pitch);
				
				state = 0; 
				setPitchInput(pitch);
				//pitchLED(pitch);
				keypadValuesSet = 1;
				printf("keypadValuesSet has been set\n");
				
				displayValue(9,4);	
					
				}
				
				position = 3;
				multiplier = 1;
				pitch = 0;
				
			}
			
			else if (input == Key_star){
				printf("Deleted current value\n");
				
				pitch = pitch - current; // might have error
				multiplier = multiplier / 10;
				
				position++;
				displayValue(-1,position);
			}
			
			else{
				printf("Value Of KeyPad is : %d\n", input);
				pitch = pitch + input * multiplier;
				multiplier = multiplier * 10;
				current = input * multiplier;
				
				displayValue(input,position);
				position--;
			}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *Timer){
	if (Timer->Instance == TIM4){
		int input = KeyPadGetValue();
		
		if(input != -1 && pressedValue < 0){
			pressedValue = input;
			//printf("PRESSED VALUE IS : %d\n", pressedValue);
		}
		
		if(input == pressedValue){
			if (pressedCounter == 150){
				printf("VALUE IS PRESSED is %d\n", pressedValue);
				pressedCounter = 0;
				pressedValue = -1;
					
			}else{
				pressedCounter++;
				//printf("VALUE OF COUNTER IS :%d\n",pressedCounter);
			}
		}
		else if (input != -1){ 
			pressedCounter =0;
			//printf("INDEX VALUE : %d/n ", input);
			pressedValue = -1;
			//printf("COUNTER IS RESETTING\n");
		}
	  
		
		
		/*
		if (state == 0 && input != -1 && keypadValuesSet < 1){
		rollInput(input); 
		}
		
		else if (state == 1 && input != -1 && keypadValuesSet < 1){
	  pitchInput(input);
		}
		}
		
		else{
		//printf("VALUES ARE SET ON THE KEYPAD NOW TRYING VALUES\n");
		
		if(rollValue > 0) {
			float32_t rollDifference = xCalibrated - rollValue;
			
			printf("ROLL DIFFERENCE %f\n",rollDifference);
			int rollDC = rollLED(rollDifference);
			float pulse_length = (41999+1) * rollDC - 1;
							
			//HWTimer.Init.Prescaler = pulse_length;
			//HAL_TIM_Base_Init(&HWTimer);
			//HAL_TIM_Base_Start_IT(&HWTimer);	
		}
		
		else if (pitchValue > 0 && rollValue == 0){
			float32_t pitchDifference = yCalibrated - pitchValue;
			printf("pitch DIFFERENCE %f\n",pitchDifference);
			
			int pitchDC = rollLED(pitchDifference);
			float pulse_length = (41999+1) * pitchDC - 1;
		  
			
			//NEED TO TEST THIS PART 
			//HWTimer.Init.Prescaler = pulse_length;
			//HAL_TIM_Base_Init(&HWTimer);
			
				
		}
		*/
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

