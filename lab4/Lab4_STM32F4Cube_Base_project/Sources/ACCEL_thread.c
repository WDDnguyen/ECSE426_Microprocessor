#include <ACCEL_thread.h>
#include <LIS3DSH.h>
#include <stm32f4xx_hal.h>
#include <cmsis_os.h>
#include "arm_math.h"
#include "math.h"

void ACCEL_thread(void const *args);

osThreadId ACCEL_thread_ID;
osThreadDef(ACCEL_thread, osPriorityNormal, 1, 0);
//extern float accelX, accelY;

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

float32_t rollAngle = 0;		
float32_t pitchAngle = 0;

void ACCEL_thread_periph_init(void) {
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	//initializer for accelerometer
	LIS3DSH_InitTypeDef accelerometer_Init;
	// filter to use if frequency of signal is too high. 
	accelerometer_Init.AA_Filter_BW = LIS3DSH_AA_BW_800;
	//sample 25 times per second
	accelerometer_Init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;
	//enable X,Y,Z coordinates
	accelerometer_Init.Axes_Enable = LIS3DSH_XYZ_ENABLE;
	//Continously sample beside of blocking. 
	accelerometer_Init.Continous_Update = LIS3DSH_ContinousUpdate_Enabled;
	//set maximum scale to +/- 2g.
	accelerometer_Init.Full_Scale = LIS3DSH_FULLSCALE_2;
	accelerometer_Init.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	//Set up the accelerometer with the configuration
	LIS3DSH_Init(&accelerometer_Init);
	
	//set up  clock for port E 
	__HAL_RCC_GPIOE_CLK_ENABLE();
	// initialize PIN setup
	GPIO_InitTypeDef GPIO_INIT;
	// set pin to specific accelerometer pin
	GPIO_INIT.Pin = LIS3DSH_SPI_INT1_PIN; 
	// trigger when falling edge of sampling
	GPIO_INIT.Mode = GPIO_MODE_IT_FALLING;
	GPIO_INIT.Pull = GPIO_PULLDOWN;
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM;
	//set up pin on PORT E with configuration
	HAL_GPIO_Init(GPIOE, &GPIO_INIT);
	
	//initialize configuration set up for interrupt of accelerometer
	LIS3DSH_DRYInterruptConfigTypeDef accelerometer_Interrupt_Init;
	//enable Interrupt for accelerometer
	accelerometer_Interrupt_Init.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	//Active high when getting signal 
	accelerometer_Interrupt_Init.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
	accelerometer_Interrupt_Init.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	LIS3DSH_DataReadyInterruptConfig(&accelerometer_Interrupt_Init);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);	
	
}

void start_ACCEL_thread(void *args) {
	ACCEL_thread_ID = osThreadCreate(osThread(ACCEL_thread), args);
}

void ACCEL_thread(void const *args) {
	//Initialize Accel Thread (Configure Accelerometer)
	ACCEL_thread_periph_init();

	while(1) {
		osSignalWait(0x00000001, osWaitForever);
		float32_t coordinate[3];
	LIS3DSH_ReadACC(coordinate);
	
	
	/*printf("X value : %f ,",coordinate[0]);
	printf("Y value : %f ,",coordinate[1]);
	printf("Z value : %f\n",coordinate[2]);
	*/		
	// might need to change to 0.981 + 0.372, 0.9792 + 0.38
	xCalibrated = coordinate[0]* 0.991 + 0.372;
	yCalibrated = coordinate[1]* 0.987	+ 0.38;
	zCalibrated = coordinate[2] * 0.9792 + 0.38;			
	
		
	/*xCalibrated = coordinate[0]* 0.846 + 0.372;
	yCalibrated = coordinate[1]* 0.846	+ 0.38;
	zCalibrated = coordinate[2] * 0.846 + 0.38;			
		*/
	//printf("X value : %f ,",coordinate[0]/gravity);
	//printf("Y value : %f ,",coordinate[1]/gravity);
	//printf("Z value : %f\n",coordinate[2]/gravity);
	/*
	printf("NOT FILTERED: %f,", xCalibrated);
	printf("%f,", yCalibrated);
	printf("%f\n", zCalibrated);
		*/
		
		
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
	

		float32_t val = 180.0/PI;
		float32_t rollCalculation = xOutputArray[0]/ sqrt(yOutputArray[0] * yOutputArray[0] + zOutputArray[0] * zOutputArray[0]);
		rollAngle = atan(rollCalculation) * val;
		
		float32_t pitchCalculation = yOutputArray[0]/ sqrt(xOutputArray[0] * xOutputArray[0] + zOutputArray[0] * zOutputArray[0]);
		pitchAngle = atan(pitchCalculation) * val;
		
		//printf("FILTERED : %f,%f\n",rollAngle,pitchAngle);
		
	}
		
	else {
		xArray[invalidCount] = coordinate[0];
		yArray[invalidCount] = coordinate[1];
		zArray[invalidCount] = coordinate[2];
		invalidCount -= 1;
		}	
	}
}
