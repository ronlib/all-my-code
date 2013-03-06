.386                                    ; create 32 bit code
.model flat, stdcall                    ; 32 bit memory model
option casemap :none                    ; case sensitive


include c:\masm32\include\windows.inc
include c:\masm32\include\kernel32.inc
;include c:\masm32\include\user32.inc

includelib c:\masm32\lib\kernel32.lib
includelib c:\masm32\lib\user32.lib
	
KERNEL32_HASH equ 0x000D4E88
KERNEL32_NUMBER_OF_FUNCTIONS equ 2
KERNEL32_CREATEFILEA_HASH equ 0x00067746
KERNEL32_CLOSEHANDLE_HASH equ 0x00067E1A




.data
	stat_str1 db "hello world", 0
	stat_str2 db "hello world2", 0
	stat_dword dword 100

	
.code ; .mypic$1

;testProc proto var1:dword, var2:dword


A:
	stat_dword2 dword 80

start:

	
	push 1
	push 2
	call [testProc]
	
	
	call geteip
	
	geteip:
		pop ebx
		
	lea eax, [stat_dword2]
	lea eax, [A - geteip + ebx]
	
	invoke ExitProcess,0


	
	
testProc proc var1:dword, var2:dword

	mov eax, [var1]
	add eax, [var2]
	
	ret

testProc endp







end start


;c:\masm32\bin\ml /coff /Fo F:\Development\Assembly\Test\MySecondAssembly.obj /c F:\Development\Assembly\Test\MySecondAssembly.asm
;c:\masm32\bin\link /subsystem:console F:\Development\Assembly\Test\MySecondAssembly.obj /out:F:\Development\Assembly\Test\MySecondAssembly.exe