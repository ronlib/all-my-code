#include <Windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	switch(fdwReason)
	{
	
	case DLL_PROCESS_ATTACH:

	OutputDebugStringA("DLL_PROCESS_ATTACH\n");

		break;
	}

	return TRUE;
}