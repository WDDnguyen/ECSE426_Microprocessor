#include "stm32f4xx_hal.h"
#include "lis3dsh.h"
#include "accelerometer.h"
#include "math.h"

ACC_Axis acc_val;
ACC_Angle acc_angle;
float acc_read[3];

// initialize accelerameter
void ACC_Initialize(void)
{
	LIS3DSH_InitTypeDef acc_init;
	LIS3DSH_DRYInterruptConfigTypeDef acc_irr;

	acc_init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;
	acc_init.AA_Filter_BW = LIS3DSH_AA_BW_50;
	acc_init.Axes_Enable = LIS3DSH_XYZ_ENABLE;
	acc_init.Continous_Update = LIS3DSH_ContinousUpdate_Disabled;
	acc_init.Full_Scale = LIS3DSH_FULLSCALE_2;
	acc_init.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	
	acc_irr.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	acc_irr.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
	acc_irr.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	
	LIS3DSH_Init(&acc_init);
	LIS3DSH_DataReadyInterruptConfig(&acc_irr);
}

// compute roll angle
float roll_cmp(void)
{
	float angle_r = atan2(acc_val.y_val, sqrt(pow(acc_val.x_val,2) + pow(acc_val.z_val,2)));
	return rangeConvert(angle_r*RAD2DEG);
}

// compute pitch angle
float pitch_cmp(void)
{
	float angle_r = atan2(acc_val.x_val, sqrt(pow(acc_val.y_val,2) + pow(acc_val.z_val,2)));
	return rangeConvert(angle_r*RAD2DEG);
}

// read acclerameter
void readData(float* acc_read)
{
	LIS3DSH_ReadACC(acc_read);
}

// update accelerameter readings with calibration data
void parseAxis(float *acc_read)
{
	acc_val.x_val = acc_read[0];
	acc_val.y_val = acc_read[1];
	acc_val.z_val = acc_read[2];
}

// convert from [-90,90] to [0,180]
float rangeConvert(float x)
{
	return x + 90;
}

// compute accelerations
void cmpACC(void)
{
	readData(acc_read);
	parseAxis(acc_read);
}

// compute angles
void cmpAngle(void)
{
	cmpACC();
	acc_angle.pitch = pitch_cmp();
	acc_angle.roll = roll_cmp();
}

// acclerometer getter
ACC_Axis getACC(void)
{
	return acc_val;
}

// angle getter
ACC_Angle getAngle(void)
{
	return acc_angle;
}
