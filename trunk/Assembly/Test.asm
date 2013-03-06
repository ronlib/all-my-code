.386 
.model flat,stdcall 
option casemap:none 
include c:\masm32\include\windows.inc 
include c:\masm32\include\kernel32.inc 
includelib c:\\masm32\lib\kernel32.lib 
include c:\masm32\include\user32.inc 
includelib c:\masm32\lib\user32.lib 

.data 
MsgBoxCaption	db "An example of Cancel,Retry,Continue",0 
MsgBoxText	db "Hello Message Box!",0 

.code 
start:
	invoke MessageBox,
			NULL,
			addr MsgBoxText,
			addr MsgBoxCaption,
			MB_ICONERROR OR MB_ABORTRETRYIGNORE

	.IF eax==IDABORT
		; Abort was pressed

	.ELSEIF eax==IDRETRY
		; Retry was pressed

	.ELSEIF eax==IDCANCEL
		; Cancel was pressed
	.ENDIF

	invoke ExitProcess,NULL
end start