#include <Windows.h>
#include <TlHelp32.h>
#include <Shlwapi.h>

#include "remote_peb_finder.h"
#include "definitions.h"

#include <iostream>


#define PEB_POSSIBLE_MIN_ADDRESS 0x7FFD0000 // On 32 bit processes only!
#define PEB_POSSIBLE_MAX_ADDRESS 0x7FFDF000

#define OUT

// Assuming the remote process is 32-bit
BOOL getRemoteModuleInjectionNecessaryData( const wchar_t * moduleName, DWORD remoteProcessId, DWORD remoteProcessPebAddress, DWORD * OUT pRemoteModuleEntryPointPointer ,DWORD * OUT pRemoteModuleBaseAddress )
{

	if (0 == remoteProcessId)
	{

		return FALSE;

	}


	HANDLE remoteProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, remoteProcessId);

	if (NULL == remoteProcess)
	{
		return FALSE;
	}


	DWORD bufferSize = 0;

	bufferSize = max(	max(	max(
		sizeof (_PEB), 
		sizeof(_TEB)), 
		sizeof(PEB_LDR_DATA)), 
		sizeof(PLDR_MODULE)
		);
	
	byte* buffer = new (std::nothrow) byte[bufferSize];



	
	DWORD remoteMemoryNumBytesRead = 0;



	// Let's read the PEB

	DWORD remotePebAddress = remoteProcessPebAddress;

	if (FALSE == ReadProcessMemory(remoteProcess, (LPCVOID)remotePebAddress, buffer, bufferSize, &remoteMemoryNumBytesRead)
		|| 0 == remoteMemoryNumBytesRead)
	{

		delete [] buffer;
		CloseHandle(remoteProcess);

		return FALSE;

	}

	
	
	
	// Let's read the LDR field of the PEB

	DWORD remoteLdrAddr = (DWORD)(((PPEB)buffer)->LoaderData);

	
	if (0 == remoteLdrAddr)
	{
		delete [] buffer;
		CloseHandle(remoteProcess);

		return FALSE;
	}



	// Let's read the PEB_LDR_DATA

	if (FALSE == ReadProcessMemory(remoteProcess, (LPCVOID)remoteLdrAddr, buffer, bufferSize, &remoteMemoryNumBytesRead)
		|| 0 == remoteMemoryNumBytesRead)
	{

		delete [] buffer;
		CloseHandle(remoteProcess);

		return FALSE;

	}


	// Finding the head link of the loaded modules list (ordered by load time)

	DWORD loadedModulesListHeadAddr = (DWORD)(((PPEB_LDR_DATA)buffer)->InLoadOrderModuleList.Flink);

	
	if (0 == loadedModulesListHeadAddr)
	{
		delete[] buffer;
		CloseHandle(remoteProcess);
		
		return FALSE;
	}


	DWORD currentRemoteModuleAddr = loadedModulesListHeadAddr;

	
	// Looping through the loaded modules in the remote list
	
	do {

		if (FALSE == ReadProcessMemory(remoteProcess, (LPCVOID)currentRemoteModuleAddr, buffer, sizeof(LDR_MODULE), &remoteMemoryNumBytesRead)
			|| 0 == remoteMemoryNumBytesRead)
		{

			delete[] buffer;
			CloseHandle(remoteProcess);

			return FALSE;

		}


		DWORD moduleNameRemoteAddr = (DWORD)(((PLDR_MODULE)buffer)->BaseDllName.Buffer);
		DWORD moduleNameLength = (DWORD)(((PLDR_MODULE)buffer)->BaseDllName.Length);





		wchar_t moduleNameBuffer[MAX_PATH + 1] = L"";

		if (0 == moduleNameRemoteAddr || 
			FALSE == ReadProcessMemory(remoteProcess, (LPCVOID)moduleNameRemoteAddr, moduleNameBuffer, (moduleNameLength + 1)*sizeof(wchar_t), &remoteMemoryNumBytesRead)
			|| 0 == remoteMemoryNumBytesRead)
		{

			delete[] buffer;
			CloseHandle(remoteProcess);

			return FALSE;

		}


		if (0 == StrCmpIW(moduleNameBuffer, L"kernel32.dll"))
		{
			
			// kernel32 LDR_MODULE found. The structure is still held under buffer.

			break;

		}

		else
		{
			// Iterating to the next module
			currentRemoteModuleAddr = (DWORD)(((PLDR_MODULE)buffer)->InLoadOrderModuleList.Flink);

		}



		

	} while (currentRemoteModuleAddr != loadedModulesListHeadAddr);



	if (currentRemoteModuleAddr != loadedModulesListHeadAddr)
	{

		// We found our module in one of our iterations. Let's return the address of where it's entry point is held

		*pRemoteModuleBaseAddress = (DWORD)(((PLDR_MODULE)buffer)->BaseAddress);
		*pRemoteModuleEntryPointPointer = (DWORD)(&(((PLDR_MODULE)currentRemoteModuleAddr)->EntryPoint));

		delete[] buffer;

		CloseHandle(remoteProcess);



		return TRUE;
			

	}



	// We don't need these any more

	delete[] buffer;

	CloseHandle(remoteProcess);
	

	
	

	return FALSE;
}




DWORD getPidByName( const char * name )
{
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	if (INVALID_HANDLE_VALUE == processesSnapshot)
	{
		return NULL;
	}

	PROCESSENTRY32 processEntry = {0};

	processEntry.dwSize = sizeof(PROCESSENTRY32);


	if (FALSE ==  Process32First(processesSnapshot, &processEntry))
	{
		CloseHandle(processesSnapshot);
		return NULL;
	}

	if (0 == StrCmpIA(processEntry.szExeFile, name))
	{
		CloseHandle(processesSnapshot);
		return processEntry.th32ProcessID;
	}

	while (TRUE == Process32Next(processesSnapshot, &processEntry))
	{
		if (0 == StrCmpIA(processEntry.szExeFile, name))
		{
			CloseHandle(processesSnapshot);
			return processEntry.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	
	
	return NULL;

}


DWORD getRemotePebAddress( DWORD remoteProcessId )
{


	if (0 == remoteProcessId)
	{

		return NULL;

	}


	HANDLE remoteProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, remoteProcessId);

	if (NULL == remoteProcess)
	{
		printf("Cannot open remote process [%d] for PROCESS_VM_READ | PROCESS_VM_WRITE\n", remoteProcessId);

		return NULL;
	}


	// Calculate the biggest buffer size we might need in order to read the remote process' memory
	DWORD bufferSize = 0;

	bufferSize = max(	max(	max(
		sizeof (_PEB), 
		sizeof(_TEB)), 
		sizeof(PEB_LDR_DATA)), 
		sizeof(PLDR_MODULE)
		);



	byte* buffer = new (std::nothrow) byte[bufferSize];

	if (0 == buffer)
	{
		// Failure on allocating the buffer
		CloseHandle(remoteProcess);
		return NULL;
	}




	// Reading addresses 7EFD0000 - 7EFD0000 in order to find the PEB

	bool foundPeb = false;
	DWORD remoteAddressRead = PEB_POSSIBLE_MIN_ADDRESS;

	DWORD remoteMemoryNumBytesRead = 0;

	while (remoteAddressRead <= PEB_POSSIBLE_MAX_ADDRESS && false == foundPeb)
	{
		// zeroing the buffer
		memset(buffer, 0, bufferSize);



		printf("Reading remote process memory in search of the PEB");
		if (FALSE == ReadProcessMemory(remoteProcess, (LPCVOID)remoteAddressRead, buffer, bufferSize, &remoteMemoryNumBytesRead))
		{
			printf(" - unable to read remote memory %x\n", remoteAddressRead);
			remoteAddressRead += 0x1000;
			continue;
		}


		if (remoteMemoryNumBytesRead > 0)
		{
			// Let's check if it's a TEB first. If so, we take the address of the PEB

			if ( (DWORD)(((_TEB*)buffer)->Tib.Self) == remoteAddressRead)
			{
				remoteAddressRead = (DWORD)(((_TEB*)buffer)->Peb);
				foundPeb = true;
				
				printf(" - found remote PEB at %x\n", remoteAddressRead);
				break;
			}
		}

		printf(" - memory read is not TEB or PEB\n");

		remoteAddressRead += 0x1000;
	}


	if (false == foundPeb)
	{
		printf("After going through all possible addresses of the PEB, non was found to be it\n");

		return NULL;
	}


	return remoteAddressRead;


}
