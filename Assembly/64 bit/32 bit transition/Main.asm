; EXTRN GetStdHandle: PROC
; EXTRN WriteFile:    PROC
; EXTRN lstrlen:      PROC
; EXTRN ExitProcess:  PROC

INCLUDE C:\masm32\include\kernel32.inc

.DATA

    hFile        QWORD 0
    msglen       DWORD 0
    BytesWritten DWORD 0
    msg          BYTE  "Hello x64 World!", 13, 10, 0

.CODE

    main PROC


     ;sub rsp, 28h
	start:
	
	int 3	
		
	push 23h	
	xor rcx, rcx
	mov ecx, code32
	push rcx
	;mov eax, 23h
	;mov [rsp + 4], eax
	
	db 48h
	retf
	
	
	; mov rax, 23h
	; mov rbx, rax
	; mov rax, code32
	; jmp rbx : eax
	
	code32:
		
		int 3
		
	; Let's return to 64 bit
	
	push 33h
	mov ecx, code64
	push rcx
	retf
	
	code64:
	

	
	xor ecx, ecx        ; exit code = 0
	call ExitProcess

    main ENDP

END