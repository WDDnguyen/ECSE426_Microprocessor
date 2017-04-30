#ifndef __ACCELEROMETER
#define __ACCELEROMETER

typedef struct{
	float x_val;
	float y_val;
	float z_val;
}ACC_Axis;

typedef struct{
	float pitch;
	float roll;
}ACC_Angle;

#define DEG2RAD PI/180
#define RAD2DEG 180/PI


#include "commen_var.h"

/**
  * @brief  Accelerometer initialization
  * @param  None
	* @retval None
  */
void ACC_Initialize(void);

/**
  * @brief  Compute roll angle
  * @param  None
	* @retval Roll angle
  */
float roll_cmp(void);

/**
  * @brief  Compute pitch angle
  * @param  None
	* @retval Pitch angle
  */
float pitch_cmp(void);

/**
  * @brief  Read accelerometer data
  * @param  Pointer to the accelerometer data
	* @retval None
  */
void readData(float* out);

/**
  * @brief  Update the pointer to the acceleration buffer
  * @param  acc_read: pointer to the accelerometer readings
	* @retval None
  */
void parseAxis(float *acc_read);

/**
  * @brief  Convert angle from [-90,90] to [0,180]
  * @param  value: input accelerations
	* @retval 2's complement of input data value
  */
float rangeConvert(float x);

/**
  * @brief  Compute accelerations
  * @param  None
	* @retval None
  */
void cmpACC(void);

/**
  * @brief  Compute pitch & roll angles
  * @param  None
	* @retval None
  */
void cmpAngle(void);

/**
  * @brief  Getter for accelerations
  * @param  None
	* @retval Acceleration values
  */
ACC_Axis getACC(void);

/**
  * @brief  Getter for angles
  * @param  None
	* @retval Pitch & roll angles
  */
ACC_Angle getAngle(void);

#endif
