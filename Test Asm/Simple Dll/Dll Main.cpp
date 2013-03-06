#include <Windows.h>


BOOL WINAPI DllMain(_In_ HANDLE _HDllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID _Reserved)
{

	OutputDebugStringA("In Simple DLL.dll DllMain()\n");

	__asm int 3

	return TRUE;
}