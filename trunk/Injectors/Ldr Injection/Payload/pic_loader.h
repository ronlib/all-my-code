#ifndef _PIC_LOADER
#define _PIC_LOADER

#include <Windows.h>

#define MAX_BACK_SEARCH_BYTES 0x200
#define MAGIC_VALUE 0xABABABAB

typedef BOOL (WINAPI * DLL_MAIN_FUNC)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
typedef HMODULE (WINAPI * LOAD_LIBRARYA_FUNC)(LPCSTR lpFileName);

typedef struct PayloadParameters
{
	DWORD MAGIC; // should be 0xABABABAB
	DLL_MAIN_FUNC m_originalDllMain;
	LOAD_LIBRARYA_FUNC m_loadLibraryA;
	char m_DllPath[MAX_PATH];

} PayloadParameters;

// Assuming a PayloadParameters struct is directly before the function in memory
BOOL WINAPI hookFunc(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
BOOL WINAPI emptyFunc();


#endif // _PIC_LOADER