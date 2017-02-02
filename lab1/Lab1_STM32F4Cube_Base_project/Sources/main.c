#include <stdio.h>
#include "arm_math.h"
#include <math.h>

typedef struct FIR_coeff{
float32_t coeffArray[5];
}FIR_coeff;

int FIR_C(float32_t* inputArray, float32_t* OutputArray, FIR_coeff* coeff, int Length, int Order);
void printStruct(FIR_coeff * coeff);
extern void FIR_asm(float32_t *inputArray, float32_t *OutputArray, int Length, struct FIR_coeff * coeff);

//Operations function provide C and DSP to provide subtraction,average,standard deviation, correlation
void FIR_C_Operations(float32_t* inputArray, float32_t* OutputArray,int Length, int Order, float32_t* subArrInC, float32_t * stdInC, float32_t * averageInC,float32_t * correlation); 
void FIR_DSP_Operations(float32_t* inputArray, float32_t* OutputArray, int Length,int Order, float32_t* subArrInDSP, float32_t * stdInDSP,float32_t * averageInDSP,float32_t *correlation);

//subfunctions for C Operation functions 
void FIR_C_Average(float32_t *Array, int length, float32_t * average);
void FIR_C_STD(float32_t *Array, int length, float32_t *averageInC, float32_t * stdInC);

int main(){

//Input Array to test
float32_t inputArray[15] = {1,2,3,4,5,6,7,7,9,10,10,12,13,14,14};

// Initializing values of coefficient,Order,Length
struct FIR_coeff coeff;
coeff.coeffArray[0] = 0.1;
coeff.coeffArray[1] = 0.15;
coeff.coeffArray[2] = 0.5;
coeff.coeffArray[3] = 0.15;
coeff.coeffArray[4] = 0.1;

int Length = sizeof(inputArray)/sizeof(inputArray[0]);
printf("Length of input array is %d\n", Length);
int Order = 4;

// FIR Filter using C
float32_t OutputArray[Length - Order];
int outputLength = sizeof(OutputArray)/sizeof(OutputArray[0]);
printf("Length of output array is %d\n", outputLength);

int result;
result = FIR_C(inputArray, OutputArray, &coeff, Length, Order);

printf("-----------------------\n");

// FIR Filter using ASM
float32_t OutputArrayASM[Length - Order];
FIR_asm(inputArray,OutputArrayASM,Length, &coeff); 

for (int i =0; i<sizeof(OutputArrayASM)/sizeof(OutputArrayASM[0]) ; i++){
	printf("Value in ASM at %i : %f\n",i,OutputArrayASM[i]);	
}
printf("-----------------------\n");

// FIR filter using CMSIS-DSP
// Might check for length of OUtput
float32_t OutputArrayDSP[Length];
uint32_t blockSize = 1;
float32_t firStateF32[10 + 5 - 1];
float32_t coefficients[5] = {0.1,0.15,0.5,0.15,0.1};

arm_fir_instance_f32 s = {Order+1, firStateF32,coefficients};
arm_fir_init_f32(&s, 5,(float32_t *)&coefficients[0],&firStateF32[0],blockSize);

// Changed from Length - Order
for(int k = 0; k < Length; k++)
{
	arm_fir_f32(&s, &inputArray[k], &OutputArrayDSP[k],blockSize);
	printf("THIS IS THE CMSIS DSP VALUE : %f\n",OutputArrayDSP[k]);
}

printf("-----------------------\n");

//PART II OPERATIONS IN C

//CORRELATION NEED FIX
//Initialize values for operations
float32_t subArrInC[Length - Order];
float32_t stdInC;
float32_t averageOfSub;
float32_t correlationInC;
FIR_C_Operations(inputArray,OutputArray,Length,Order,subArrInC,&stdInC,&averageOfSub,&correlationInC);

printf("-----------------------\n");

//  PART II CMSIS DSP Operations
// CORRELATION NEED FIX
//Initialize values for operations
float32_t subArrInDSP[Length - Order];
float32_t stdInDSP;
float32_t averageInDSP;
float32_t correlation[2*Length - 1];

FIR_DSP_Operations(inputArray,OutputArray,Length,Order,subArrInDSP,&stdInDSP,&averageInDSP,correlation);
	
	return 0;
}

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
                printf("C Output AT INDEX %d is : %f\n", j, OutputArray[j]);
        }
        return 0;
}

void FIR_C_Operations(float32_t* inputArray, float32_t* OutputArray,int Length,int Order,float32_t* subArrInC, float32_t * stdInC, float32_t * averageInC,float32_t * correlation){
	int LengthOfSubArr = sizeof(subArrInC)/sizeof(subArrInC[0]);
	int N = Length - Order;
	
	// SUBTRACTION
	for (int i =0; i <N; i++){
	subArrInC[i] = OutputArray[i] - inputArray[i];
	printf("SUB ARRAY IN C  : %f\n",subArrInC[i]);	
	}
	
	//AVERAGE IN C
	FIR_C_Average(subArrInC,N,averageInC);
	
	//STANDARD DEVIATION IN C 
	FIR_C_STD(subArrInC,N,averageInC,stdInC);
	
	// CORRELATION IN C
	
	//ASSUMING Average all input (20) and average all output (16),
	//ASSUMING STD input (16) and STD output (16);
	float32_t inputAverage;
	float32_t outputAverage;
	FIR_C_Average(inputArray,Length,&inputAverage);
	FIR_C_Average(OutputArray,N,&outputAverage);
	
	float32_t sumOfNumerator;
	float32_t denominator;
	float32_t inputSTD;
	float32_t outputSTD;
	FIR_C_STD(inputArray,N,&inputAverage,&inputSTD);
	FIR_C_STD(OutputArray,N,&outputAverage,&outputSTD);
	
	for (int i =0; i < N ; i++){
		sumOfNumerator += (inputArray[i] - inputAverage) * (OutputArray[i] - outputAverage);
}
	
	//Denominator
	denominator = (N-1) * inputSTD * outputSTD;
	
	*correlation = sumOfNumerator/denominator; 
	printf("THIS IS THE CORRELATION IN C : %f\n",*correlation);
	
}

void FIR_DSP_Operations(float32_t* inputArray, float32_t* OutputArray, int Length, int Order, float32_t* subArrInDSP, float32_t * stdInDSP, float32_t * averageInDSP,float32_t * correlation){
uint32_t N = Length - Order;

arm_sub_f32(OutputArray,inputArray,subArrInDSP,N);
for (int i = 0 ; i < N ; i++){
printf("THIS IS THE VALUE IN DSP LIBRARY : %f\n", subArrInDSP[i]);
}


//Average in DSP
arm_mean_f32(subArrInDSP,N,averageInDSP);
printf("VALUE OF AVERAGE IN DSP : %f\n",*averageInDSP);

//Standard deviation in DSP
arm_std_f32(subArrInDSP,N,stdInDSP);
printf("STD OF DSP IS : %f\n", *stdInDSP);

arm_correlate_f32(inputArray,Length,OutputArray,N,correlation);
	
float32_t inputSum;
float32_t outputSum;
for (int i = 0; i<Length;i++){
	inputSum += inputArray[i] * inputArray[i];
}	
for (int k =0; k<N ;k++){
	outputSum += OutputArray[k] * OutputArray[k];
}	

float32_t value = sqrt(inputSum * outputSum);
correlation[Length-1] = correlation[Length-1]/value;

//Correlation NEED TO FIX
printf("CORRELATION VALUE IN DSP IS : %f\n",correlation[Length-1]);

}
void FIR_C_Average(float32_t *Array, int length,float32_t *average){
	float32_t avg = 0;
	printf("THIS IS THE LENGTH OF SUB ARRAY : %d\n", length);
	for (int i = 0; i< length; i++){
	avg += Array[i];
	}
	avg = avg / length;
	printf("AVERAGE IN C IS : %f\n\n",avg);
	*average = avg;
}

void FIR_C_STD(float32_t *Array, int length, float32_t *averageInC, float32_t * stdInC){
	for (int i = 0; i<length; i++){
	*stdInC += pow((Array[i] - *averageInC),2);
	}
	*stdInC = *(stdInC)/(length-1);
	*stdInC = sqrt(*stdInC);
	printf("THIS IS THE STANDARD DEVIATION IN C : %f\n\n",*stdInC);
}