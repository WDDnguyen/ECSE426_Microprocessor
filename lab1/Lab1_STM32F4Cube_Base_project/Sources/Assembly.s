	AREA text, CODE, READONLY
	EXPORT FIR_asm
FIR_asm
	
	PUSH{R4-R12}
	;initialize value
	SUB R2,R2, #4 ; Length - Order
	MOV R5, #0 ;j
	
loop
	VLDR.f32 S4, =0.0  ;y =0 ;
	MOV R4, #0  ;i
	
loop2
	;k = j + i
	ADD R6,R5,R4
	
	;inputArray[k]
	ADD R7, R0, R6, LSL #2
	;load register address to float register s8
	VLDR.f32 S6, [R7]
	
	;coeffArray[i]
	ADD R8,R3,R4,LSL #2
	;load register address to float register s8
	VLDR.f32 S8,[R8]
	
	;temp = inputArray[k] * coeffArray[i]
	VMUL.f32 S1, S8,S6
	;y = y + temp
	VADD.f32 S4, S1,S4
	;i++
	ADDS R4,#1
	;check if i < order + 1
	CMP R4,#5
	BNE loop2
	
	;OutputArray[j] = y;
	ADD R7,R1,R5, LSL #2
	VSTR.f32 S4,[R7] 
	;j++
	ADD R5,#1
	;check if j < length - order
	CMP R5,R2
	BLT loop
	POP{R4-R12}
	BX LR
	END
	

	
	
	