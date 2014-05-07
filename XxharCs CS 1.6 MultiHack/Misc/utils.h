#ifndef __UTILS_H__
#define __UTILS_H__


//////////////////////////////////////////////////////////////////////////

#define		utilsVAlloc(x)	VirtualAlloc(0, x, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE)
#define		utilsVFree(x)	VirtualFree(x, 0, MEM_RELEASE)

//////////////////////////////////////////////////////////////////////////
void	utilsGetInstructionLength(LPVOID address, DWORD* osizeptr);
DWORD	utilsGetInstructionLength(LPVOID addr);
DWORD	utilsGetWholeCodeSize(LPVOID start);

BOOL	utilsCompareData(const BYTE* pData, const BYTE* bMask, const char* pszMask);
DWORD	utilsFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * pszMask);
void	utilsCreateJMP(DWORD at, DWORD to);
void	utilsCreateCall(DWORD at,DWORD to);
DWORD	utilsRedirectCall(DWORD at, DWORD to);
DWORD	utilsReconstructJMP(DWORD dwAddress);

DWORD	utilsCopyRange(DWORD dwFrom, DWORD dwTo);
void	utilsWrapMemory(DWORD dwAddress, DWORD dwNew, DWORD dwMinLen);

void	utilsDPrint(char *fmt, ...);

#endif