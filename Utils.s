	AREA Utils_s,CODE,READONLY
		
	EXPORT firstProcess
	EXPORT contextSwitch
	
	
firstProcess	PROC
				;mov r1,#2
				;msr control,r1 ;R1 to PSP
				;mov sp,r0
				;pop {lr}
				;pop {pc}
				
				mov sp,r0
				pop {r4-r12,pc}
				
			    ENDP
				
				
				
contextSwitch PROC
				;;sichern von p1
				
				push {r4-r12,lr}  
				mov r4, sp
				str r4,[r1]
				
				;;holen von p2
				mov sp,r0
				pop {r4-r12,pc}
				ENDP



		
	END
