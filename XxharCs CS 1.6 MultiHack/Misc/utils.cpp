#include <windows.h>
#include "utils.h"
#include "opcodes.h" // only once



//////////////////////////////////////////////////////////////////////////
void	utilsGetInstructionLength(LPVOID address, DWORD* osizeptr)
{
	BYTE* iptr0 = (BYTE*)address;
	DWORD f = 0;
	BYTE b, mod, rm;
	BYTE* iptr = iptr0;

prefix:
	b = *iptr++;
	f |= table_1[b];

	if (f & C_FUCKINGTEST)
		if (((*iptr) & 0x38) == 0x00)
			f = C_MODRM + C_DATAW0;     // TEST
		else
			f = C_MODRM;				// NOT,NEG,MUL,IMUL,DIV,IDIV

	if (f & C_TABLE_0F)
	{
		b = *iptr++;
		f = table_0F[b];
	}

	if (f == C_ERROR)
	{
		*osizeptr = C_ERROR;
		return;
	}

	if (f & C_PREFIX)
	{
		f &=~ C_PREFIX;
		goto prefix;
	}

	if (f & C_DATAW0) 
		if (b & 0x01) 
			f |= C_DATA66; 
		else 
			f |= C_DATA1;

	if (f & C_MODRM)
	{
		b=*iptr++;
		mod=b&0xC0;
		rm=b&0x07;
		if (mod!=0xC0)
		{
			if (f & C_67)       // modrm16
			{
				if (mod == 0x40) f |= C_MEM1;
				if ((mod == 0x00) && (rm == 0x06)) f |= C_MEM2;
				if (mod == 0x80) f |= C_MEM2;
			}
			else				// modrm32
			{
				if (mod == 0x80) f |= C_MEM4;
				if (mod == 0x40) f |= C_MEM1;
				if (rm == 0x04) rm = (*iptr++) & 0x07;    // rm<-sib.base
				if ((rm == 0x05) && (mod == 0x00)) f |= C_MEM4;
			}
		}
	} // C_MODRM

	if (f & C_MEM67) if (f & C_67) f |= C_MEM2; else f |= C_MEM4;
	if (f & C_DATA66) if (f & C_66) f |= C_DATA2; else f |= C_DATA4;

	if (f & C_DATA1) iptr++;
	if (f & C_DATA2) iptr += 2;
	if (f & C_DATA4) iptr += 4;

	if (f & C_MEM1) iptr++;
	if (f & C_MEM2) iptr += 2;
	if (f & C_MEM4) iptr += 4;

	*osizeptr=iptr-iptr0;
}

//////////////////////////////////////////////////////////////////////////
DWORD	utilsGetInstructionLength(LPVOID addr)
{
	DWORD len;

	utilsGetInstructionLength((DWORD*)addr, &len);

	return len;
}

//////////////////////////////////////////////////////////////////////////
DWORD	utilsGetWholeCodeSize(LPVOID start)
{
	DWORD size = 0, totalsize = 0;

	while(TRUE){ // dangerous! scanning till ret/retn
		utilsGetInstructionLength((DWORD*)start, &size);

		if(size == C_ERROR)
			break;

		totalsize += size;

		if(*(LPBYTE)start == 0xC3 || *(LPBYTE)start == 0xC2)
			break;

		*(DWORD*)&start += size;
	}

	return totalsize;
}

//////////////////////////////////////////////////////////////////////////
BOOL utilsCompareData(const BYTE* pData, const BYTE* bMask, const char* pszMask)
{
	for(;*pszMask; ++pszMask, ++pData, ++bMask)
		if(*pszMask == 'x' && *pData !=* bMask ) 
			return FALSE;
	return (*pszMask) == 0;
}

//////////////////////////////////////////////////////////////////////////
DWORD utilsFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * pszMask)
{
	for(DWORD i=0; i < dwLen; i++)
		if(utilsCompareData((BYTE*)( dwAddress+i ), bMask, pszMask ))
			return (DWORD)(dwAddress + i);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
DWORD utilsReconstructJMP(DWORD dwAddress)
{
	DWORD  dwJMP = *(DWORD*)(dwAddress + 1);
	dwJMP += dwAddress + 5;
	return dwJMP;
}

//////////////////////////////////////////////////////////////////////////
void utilsCreateJMP(DWORD at, DWORD to)
{
	DWORD		dwOldProtect = 0;

	VirtualProtect((DWORD*)at, 5, 0x40, &dwOldProtect);
	*(BYTE*)(at)	= 0xE9;
	*(DWORD*)(at+1)	= to - (at + 5);
	VirtualProtect((DWORD*)at, 5, dwOldProtect, &dwOldProtect);
}

//////////////////////////////////////////////////////////////////////////
void utilsCreateCall(DWORD at, DWORD to)
{
	DWORD dwOldProtect;

	VirtualProtect((DWORD*)at, 5, 0x40, &dwOldProtect);
	*(BYTE*) (at)	= 0xE8;
	*(DWORD*)(at+1)	= to - (at + 5);
	VirtualProtect((DWORD*)at, 5, dwOldProtect, &dwOldProtect);
}

//////////////////////////////////////////////////////////////////////////
DWORD utilsRedirectCall(DWORD at, DWORD to)
{
	DWORD dwRetval = utilsReconstructJMP(at);
	utilsCreateCall(at, to);
	return dwRetval;
}

//////////////////////////////////////////////////////////////////////////
DWORD utilsCopyRange(DWORD dwFrom, DWORD dwTo)
{
	DWORD dwOldProtect = 0;
	DWORD dwLen = dwTo - dwFrom;
	VirtualProtect((DWORD*)dwFrom, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	
	void *pCopy = VirtualAlloc(0, dwLen, 0x1000|0x2000, 0x40);
	utilsWrapMemory(dwFrom, (DWORD)pCopy, dwLen); //fix calls/jmps
	VirtualProtect((DWORD*)dwFrom, dwLen, dwOldProtect, &dwOldProtect);
	return (DWORD)pCopy;
}

//////////////////////////////////////////////////////////////////////////
void utilsWrapMemory(DWORD dwAddress, DWORD dwNew, DWORD dwMinLen)
{
	DWORD dwLen =0;

	while(dwLen < dwMinLen){
	
		DWORD dwOpcodeLen = utilsGetInstructionLength((LPVOID)(dwAddress + dwLen));
		
		memcpy((void*)(dwNew+dwLen),(void*)(dwAddress+dwLen), dwOpcodeLen);
		
		if(*(BYTE*)(dwAddress + dwLen) == 0xE9 )
			utilsCreateJMP(dwNew + dwLen, utilsReconstructJMP(dwAddress + dwLen));

		if( *(BYTE*)(dwAddress + dwLen) == 0xE8 )
			utilsCreateCall(dwNew+dwLen, utilsReconstructJMP(dwAddress + dwLen));

		dwLen += dwOpcodeLen;
	}
}

//////////////////////////////////////////////////////////////////////////
void utilsDPrint(char *fmt, ...)
{
	va_list vl;
	char msg[1024], total[1024];
	char prefix[] = "killbill: ";

	va_start(vl, fmt);
	wvsprintf(msg, fmt, vl);
	va_end(vl);
	lstrcpy(total, prefix);
	lstrcat(total, msg);

	OutputDebugStringA(total);
}






