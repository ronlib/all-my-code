#pragma once

#include <Windows.h>

extern "C" DWORD __stdcall testProc (DWORD, DWORD);

extern "C" DWORD __stdcall findImageMappedSize(DWORD);

extern "C" DWORD __stdcall loadFromBuffer(DWORD);

extern "C" DWORD __stdcall getDllMainAddress(DWORD);