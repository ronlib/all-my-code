#include <Windows.h>

#include <stdio.h>


#define EM(a) __asm __emit (a)

#define X64_Start_with_CS(_cs) \
{ \
	EM(0x6A) EM(_cs)                     /*  push   _cs                   */ \
	EM(0xE8) EM(0) EM(0) EM(0) EM(0)     /*  call   $+5                   */ \
	EM(0x83) EM(4) EM(0x24) EM(5)        /*  add    dword [esp], 5        */ \
	EM(0xCB)                             /*  retf                         */ \
}

#define X64_End_with_CS(_cs) \
{ \
	EM(0xE8) EM(0) EM(0) EM(0) EM(0)     /*  call   $+5                   */ \
	EM(0xC7) EM(0x44) EM(0x24) EM(4)     /*                               */ \
	EM(_cs) EM(0) EM(0) EM(0)            /*  mov    dword [rsp + 4], _cs  */ \
	EM(0x83) EM(4) EM(0x24) EM(0xD)      /*  add    dword [rsp], 0xD      */ \
	EM(0xCB)                             /*  retf                         */ \
}

#define X64_Start() X64_Start_with_CS(0x33)
#define X64_End() X64_End_with_CS(0x23)

int main()
{

	__asm int 3

	X64_Start();

	__asm int 3

	X64_End();

	__asm int 3
	OutputDebugStringA("In loaded DLL!\n");
	
	return 0;
}