; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい�
    include c:\masm32\include\masm32rt.inc
; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい�

.code

start:

	int 3

    ; call getAddress
	
	; getAddress:
		; pop ebx
	
	push 23h
	mov ebx, continue
	push ebx

	retf
	continue:
		int 3
	
	
    inkey
    exit
	
    ret



; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい�

end start
