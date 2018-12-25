	AREA Utils_s,CODE,READONLY
		
	EXPORT firstProcess
	EXPORT contextSwitch
	
	
	
	firstProcess	PROC
				mov r1,#2
				msr control,r1 //R1 to PSP
				mov sp,r0
				pop {lr}
				pop {pc}		
				ENDP
				
				
				
	contextswitch PROC
				;sichern von p1
				
				push {r4-r12,lr}  
				mov r4, sp
				str r4,[r1]
				
				;holen von p2
				mov sp,r0
				pop {r4-r12,pc}
				ENDP



		
	END
	
	
	
	
	
	
	
	
	
	    export firstProc    ; make routine callable from C-code
    mov r1, sp          ; save current msp
    mov sp, r0          ; change sp to first process' sp
    pop {lr}            ; get entry-point from stack
    mov r0, sp
    msr psp, r0         ; write current sp to psp, as we're using this one later
    msr msp, r1         ; restore msp
    mov r1, #2
    msr control, r1     ; switch to psp