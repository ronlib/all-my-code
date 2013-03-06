.386                                    ; create 32 bit code
.model flat, stdcall                    ; 32 bit memory model
option casemap :none                    ; case sensitive

include c:\masm32\include\windows.inc
include c:\masm32\include\kernel32.inc

includelib c:\masm32\lib\kernel32.lib

.code





testLibProc proc

	ret

testLibProc endp

end