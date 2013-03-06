#include <Windows.h>


DWORD injectPayloadToRemoteProcess(DWORD remoteProcessId, byte * bufferToInject, DWORD bufferLength);

BOOL getRemoteModuleInjectionNecessaryData(const wchar_t * moduleName, DWORD remoteProcessId, DWORD remoteProcessPebAddress, DWORD * OUT pRemoteModuleEntryPointPointer ,DWORD * OUT pRemoteModuleBaseAddress );

DWORD getRemotePebAddress (DWORD remoteProcessId);

DWORD getPidByName(const char * name);