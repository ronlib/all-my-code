#include <stdio.h>
#include "ldr_injector.h"

int main(int argc, char * argv[])
{

	if (3 > argc)
	{
		printf("Usage: injector <destination process> <injected dll>\n");
		return 0;
	}

	if (false == injectLdr(argv[1], L"kernel32.dll", argv[2]))
	{

		printf("Unable to inject DLL to remote process\n");
		return 1;

	}


	Sleep(INFINITE);

	return 0;

}