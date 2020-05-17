#pragma once
#include <Windows.h>

class SigScanner
{
public:
	DWORD FindAddress(BYTE* bMask, int nLength, DWORD dwBaseAddress, DWORD dwLength);
};

