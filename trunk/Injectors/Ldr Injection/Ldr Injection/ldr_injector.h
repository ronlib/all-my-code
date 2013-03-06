#ifndef _LDR_INJECTOR
#define _LDR_INJECTOR

#include <Windows.h>
#include "definitions.h"
#include "../Payload/pic_loader.h"

bool injectLdr (const char * targetProcessName, const wchar_t * moduleName, const char * injectedDllPath);

bool injectAndHook(DWORD remoteProcessId, DWORD remoteHookedAddress, DWORD remoteModuleBaseAddress, const char * injectedDllPath);



#endif // _LDR_INJECTOR