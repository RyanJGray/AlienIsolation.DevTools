#include "SigScanner.h"

/* Syntax                                *

*                                       *

*   DWORD FindAddress(                  *

*         BYTE* bMask,                  *

*         int nLength,                  *

*         DWORD dwBaseAddress,          *

*         DWORD dwLength,               *

*   );                                  *

*                                       *

* Parameters                            *

*                                       *

*   bMask                               *

*     Pointer to a BYTE-array holding   *

*     the signature to scan for, 0x99   *

*     is wildcard-byte.                 *

*                                       *

*   nLength                             *

*     How many bytes the signature is.  *

*                                       *

*   dwBaseAddress                       *

*     Address to start searching from.  *

*                                       *

*   dwLength                            *

*     How many addresses to scan thru.  *

*                                       *

*                                       *

* Return                                *

*                                       *

*   If successfull, returns the address *

*   that was found. If unsuccessfull,   *

*   returns 0.                          *

* sigscan.cpp        Signature searcher *

****************************************/

//function which checks if current offset is the sig

bool DataCompare(const BYTE* bData, const BYTE* bMask, int nLength)
{

	for (int i = 0; i < nLength; i++) { //search the whole length

		if ((bData[i] != bMask[i]) && (bMask[i] != 0x99)) //if they don't match & the mask is not 0x99

			return false; //they are not equal, return false

	}

	return true; //they are equal, return true

}

//function to search for signature

DWORD SigScanner::FindAddress(BYTE* bMask, int nLength, DWORD dwBaseAddress, DWORD dwLength)
{

	for (DWORD i = 0; i < (dwLength - nLength); i++) //while we're searching

		if (DataCompare((BYTE*)(dwBaseAddress + i), bMask, nLength)) //compare bytes

			return (DWORD)(dwBaseAddress + i); //address found! return it

	return 0; //no address found, return nothing

}