#include <stdio.h>
#include "definitions.h"


typedef BOOL (WINAPI *t_DllMain)(
	__in  HINSTANCE hinstDLL,
	__in  DWORD fdwReason,
	__in  LPVOID lpvReserved
	);


int main()
{

	//printf("Own image size: %x\n", findImageMappedSize((DWORD)GetModuleHandle(NULL)));

	HANDLE diskModuleFileHandle = CreateFileA("Simple Dll.dll", 
		GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == diskModuleFileHandle)
	{

		printf("Unable to open file\n");

		return 1;
	}

		
	HANDLE fileMappingHandle = CreateFileMappingA(diskModuleFileHandle, NULL, 
		PAGE_READWRITE, 0, 0xFFFFFF, NULL);


	if (INVALID_HANDLE_VALUE == fileMappingHandle || NULL == fileMappingHandle)
	{
		printf("Unable to map file: %x\n", GetLastError());
		CloseHandle(diskModuleFileHandle);

		return 1;
	}


	
	
	
	LPVOID moduleFileView = MapViewOfFile(fileMappingHandle, FILE_MAP_READ, 0, 0, 0);
	
	if (NULL == moduleFileView)
	{
		printf("Unable to view file mapping: %d. fileMappingHandle: %x\n", GetLastError(), fileMappingHandle);
		CloseHandle(fileMappingHandle);
		CloseHandle(diskModuleFileHandle);

		return 1;
	}


	printf("Image is loaded at %x\n", (DWORD)moduleFileView);

	DWORD loadedImageAddress = loadFromBuffer((DWORD)moduleFileView);

	if (0 != loadedImageAddress)
	{
		t_DllMain dllMainPtr = (t_DllMain)getDllMainAddress(loadedImageAddress);

		__asm int 3
		dllMainPtr((HINSTANCE)loadedImageAddress, DLL_PROCESS_ATTACH, NULL);
	}

	
	
	return 0;
}