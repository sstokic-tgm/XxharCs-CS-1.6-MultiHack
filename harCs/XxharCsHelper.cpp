#include "XxharCsHelper.h"

bool XxharCsHelper::bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for(;*szMask;++szMask,++pData,++bMask)
		if(*szMask=='x' && *pData!=*bMask)  
			return 0;
	return (*szMask) == NULL;
}

DWORD XxharCsHelper::FindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask)
{
	for(DWORD i=0; i<dwLen; i++)
		if (bCompare((BYTE*)(dwAddress+i),bMask,szMask))  
			return (DWORD)(dwAddress+i);
	return 0;
}

void* XxharCsHelper::createJMPx32(BYTE* src, BYTE* dst, const int len)
{
	DWORD dwOldProtect;

	BYTE* jmp = (BYTE*)malloc(len + 5);

	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	memcpy(jmp, src, len);
	jmp += len;
	jmp[0] = BYTECODE_JMP;
	*(DWORD*)(jmp + 1) = (DWORD)((src+len) - jmp) - 5;
	memset(src, BYTECODE_NOP, len);
	src[0] = BYTECODE_JMP;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;

	VirtualProtect(src, len, dwOldProtect, &dwOldProtect);

	return(jmp-len);
}

void XxharCsHelper::removeJMPx32(BYTE* src, BYTE* jmp, const int len)
{
	DWORD dwOldProtect;

	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	memcpy(src, jmp, len);

	VirtualProtect(src, len, dwOldProtect, &dwOldProtect);
}