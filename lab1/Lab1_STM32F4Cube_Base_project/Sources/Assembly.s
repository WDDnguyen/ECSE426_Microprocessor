	AREA text, CODE, READONLY
	EXPORT FIR_asm
FIR_asm
	
	PUSH{R4-R12}
	
	SUB R2,R2, #4
	MOV R5, #0  ;j
	
loop
	VLDR.f32 S4, =0.0
	MOV R4, #0  ;i
	
loop2
	;k = j + i
	ADD R6,R5,R4
	
	;inputArray[k]
	ADD R7, R0, R6, LSL #2
	VLDR.f32 S6, [R7] ;x4
	
	;coeffArray[i]
	ADD R8,R3,R4,LSL #2
	VLDR.f32 S8,[R8]
	
	;y += inputArray[k] * coeffArray[i]
	VMUL.f32 S1, S8,S6
	VADD.f32 S4, S1,S4
	ADDS R4,#1
	CMP R4,#5
	BNE loop2
	
	;OutputArray[j] = y;
	ADD R7,R1,R5, LSL #2
	VSTR.f32 S4,[R7] 
	
	ADD R5,#1
	CMP R5,R2
	BLT loop
	POP{R4-R12}
	BX LR
	END
	

	
	
	