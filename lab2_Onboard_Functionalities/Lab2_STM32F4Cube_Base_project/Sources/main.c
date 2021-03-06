/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "arm_math.h"
#include <math.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void HAL_SYSTICK_Callback(void); 
/* USER CODE BEGIN PFP */
 
typedef struct FIR_coeff{
float32_t coeffArray[5];
}FIR_coeff;

int FIR_C(float32_t* inputArray, float32_t* OutputArray, FIR_coeff* coeff, int Length, int Order);

/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

// initialize flag for sysTick
volatile int sysTickFlag;

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	ADC_Init();
	GPIO_Init();
	
	float32_t inputArray[5];
	// Initializing coefficients,Order,Length,Arrays for FIR filter
	struct FIR_coeff coeff;
	coeff.coeffArray[0] = 0.1;
	coeff.coeffArray[1] = 0.15;
	coeff.coeffArray[2] = 0.5;
	coeff.coeffArray[3] = 0.15;
	coeff.coeffArray[4] = 0.1;

	int Length = 5;
	int Order = 4;
	int invalidCount = 4;
	
	float32_t OutputArray[Length - Order];

	//alarm variables
	int overHeatCounter = 0;
	int clockTimer;

	//7 segment display 
	int position = 3;
	int toggle =0; 
	int sampleTime = 0;
	
/* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			//when in sysTick
			if (sysTickFlag == 1){
				sysTickFlag = 0;
				
				//acquire value from ADC in celsius 
				
				if (sampleTime++ == 100){
					sampleTime = 0;
				float celsius = ADC_getValue();
				int FIROutput;
				
				//FIR Filtering temperature value
				// If there is more than 4 value then start acquiring a value for output using moving average
				if (invalidCount == 0){
				for (int i = 5; i < 5; i++){ 
					inputArray[5-i] = inputArray[4-i];
				}
				inputArray[0] = celsius;
				
				FIROutput = FIR_C(inputArray, OutputArray, &coeff, Length, Order);
				float fahrenheit = convertToFahrenheit(OutputArray[0]);
				printf( "Value in Celsius : %f, Value in Fahrenheit : %f\n", OutputArray[0], fahrenheit);
				
				}
				// If there is less than 4 input at the start then add into the array and wait till there is 5 value 
				else {
					inputArray[invalidCount] = celsius;
					invalidCount -= 1;
					printf("FIRST 4 POLL DOESNT GIVE VALUE : %d \n", invalidCount);
				}
			}
				//lower frequency of alarm clock to make it loop in a circle.
				if (clockTimer++ == 100){
					clockTimer = 0;
				
					//Overheating Alarm when threshold is passed
					if (OutputArray[0]  > 33){
						overHeatAlarm(overHeatCounter);
						overHeatCounter++;
						// loop between 0-3 if stay too long
						overHeatCounter = overHeatCounter % 4;  
					}
					else {
						overHeatAlarm(-1);
					}
				}
				
				// When holding the blue button on the board, convert the value into celsius 
				if ( blueButtonPressed() == 1){
				  toggle = 1;					
				}
				else {
					toggle = 0;
				}
			
				int value;
				
				//acquire the value of the last 
				if (position == 3)
				{
					// if blueButton is pressed then change value to fahrenheit
					if (toggle == 0){
					value = OutputArray[0] * 10;
					}
					else {
					value = convertToFahrenheit(OutputArray[0]) * 10;
				}
				}			
			
				// set number value in XX.Y format
				if (position > 0){
				int out = value % 10; 
				value = value / 10;
				displayValue(out, position);	
				position = position -1;
				}
				else {
				position = 3;
				}
		

			}
			
  /* USER CODE END WHILE */
	 
  /* USER CODE BEGIN 3 */
	
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
int FIR_C(float* inputArray, float* OutputArray, FIR_coeff* coeff, int Length,int Order){
        int i,j,k;
        float y;

        for (j = 0 ; j < Length - Order; j++){
                y = 0;
                for ( i = 0 ; i < Order+1; i++){
                        k  = j+i;
                        y += inputArray[k] * coeff->coeffArray[i];
        }

                OutputArray[j] = y;
                
        }
        return 0;
}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
