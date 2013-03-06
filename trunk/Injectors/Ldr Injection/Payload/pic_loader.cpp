#include <Windows.h>

#include "pic_loader.h"

#pragma code_seg(".HOOK$A")

BOOL WINAPI hookFunc(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	DWORD searchIndex = 0;
	BYTE * magicIndex = 0;
	DWORD backSearchCounter = 0;
	PayloadParameters * pPayloadParameters = NULL;

	__asm
	{

		call $+0x5
		pop eax
		mov [searchIndex], eax
		
	}

	magicIndex = (BYTE*) searchIndex;

	/*__asm int 3*/

	while (backSearchCounter++ < MAX_BACK_SEARCH_BYTES &&  MAGIC_VALUE != *((DWORD*)(searchIndex--)));


	if (MAX_BACK_SEARCH_BYTES == backSearchCounter)
	{

		// Not good
		return TRUE;

	}

	++searchIndex;

	pPayloadParameters = (PayloadParameters * ) searchIndex;

	pPayloadParameters->m_loadLibraryA(pPayloadParameters->m_DllPath);

	return pPayloadParameters->m_originalDllMain(hinstDLL, fdwReason, lpvReserved);
	
}

#pragma code_seg(".HOOK$B")

BOOL WINAPI emptyFunc()
{
	return TRUE;
}

#pragma code_seg()