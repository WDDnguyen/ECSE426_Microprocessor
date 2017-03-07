/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "arm_math.h"
	 
/* USER CODE BEGIN Includes */
#define SEG7_A GPIO_PIN_7			//Pin PE7  brown cable
#define SEG7_B GPIO_PIN_8			//Pin PE8  brown cable
#define SEG7_C GPIO_PIN_9			//Pin PE9  yellow cable
#define SEG7_D GPIO_PIN_10		//Pin PE10 yellow cable
#define SEG7_E GPIO_PIN_11		//Pin PE11 gray cable
#define SEG7_F GPIO_PIN_12		//Pin PE12 gray cable
#define SEG7_G GPIO_PIN_13		//Pin PE13 green cable
#define SEG7_DP GPIO_PIN_14		//Pin PE14 green cable

#define SEG7_OUT1 GPIO_PIN_2		//Pin PE2 white cable
#define SEG7_OUT2 GPIO_PIN_4		//Pin PE4 red cable
#define SEG7_OUT3 GPIO_PIN_5		//Pin PE5 brown cable
#define SEG7_OUT4 GPIO_PIN_6		//Pin PE6 white cable 

#define LED1 GPIO_PIN_12 //Green
#define LED2 GPIO_PIN_13 //Orange
#define LED3 GPIO_PIN_14 //Red
#define LED4 GPIO_PIN_15 //Blue

#define blueButton GPIO_PIN_0 // blue button 

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void GPIO_Init(void);
int rollLED(float32_t rollDifference);
int pitchLED(float32_t pitchDifference);
void displayValue();
void resetDisplay();


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
