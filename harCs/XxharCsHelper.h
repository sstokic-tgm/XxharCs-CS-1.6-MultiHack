#ifndef __XXHARCSHELPER_H__
#define __XXHARCSHELPER_H__

#include <Windows.h>

#define BYTECODE_JMP 0xE9
#define BYTECODE_NOP 0x90

class XxharCsHelper
{
public:
		
	void* createJMPx32(BYTE* src, BYTE* dst, const int len);
	void removeJMPx32(BYTE* src, BYTE* jmp, const int len);
	bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
	DWORD FindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask);

};
#endif