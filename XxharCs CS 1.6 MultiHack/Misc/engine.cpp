#include <windows.h>
#include "engine.h"
#include "utils.h"

//////////////////////////////////////////////////////////////////////////
DWORD g_dwHW_Base;		//hw.dll code base
DWORD g_dwHW_Len;		//hw.dll code len

DWORD g_dwReturn;				//real HW_DecodeDll return address
DWORD g_dwCL_ParseSound_Begin;
DWORD g_dwCL_ParseSound_Call;
DWORD g_dwCL_ParseSound_End;
DWORD g_dwSvc_Table;
DWORD g_dwTimeOfs;

DWORD g_dwJmp;
DWORD g_dwCall;
DWORD g_dwPtr;

HL_DecodeFunc_t			pHL_DecodeFunc;
sound_t					pSound;
svc_entry_s*			pSvcDispatch;
BYTE					g_FixValue[5];
DWORD					g_dwFixaddr = NULL;
double*					g_globalTime;


//////////////////////////////////////////////////////////////////////////
void playerSound(int index, const float*const origin, const char*const sample);
void S_StartDynamicSound( int entnum, int entchannel, char* sample, float* origin, float volume, float attenuation, int timeofs , int pitch )
{
	
	if (entnum >= 1 && entnum < 32 && sample && sample[0] && origin)
			playerSound(entnum,origin,sample);

	(*pSound)(entnum,entchannel,sample,origin,volume,attenuation,timeofs,pitch);
}

//////////////////////////////////////////////////////////////////////////
DWORD engineHookSvcEntry( DWORD dwBegin,	 // begin of the parser
					  DWORD dwEnd,		 // end of the parser
					  DWORD dwCall,	 // call to the original handler
					  DWORD dwHandler,  // our handler
					  int iPos)			 // svc_table element ##x
					  
{	
	//Make a copy of the original Handler
	DWORD dwMy_Handler			= utilsCopyRange(dwBegin, dwEnd);
	//Hook Svc_Table ( hw.dll )
	pSvcDispatch[iPos].pfnHandler = (DWORD)dwMy_Handler;
	//Redirect Call
	return utilsRedirectCall((dwMy_Handler+dwCall), dwHandler);
}

//////////////////////////////////////////////////////////////////////////
void engineFindOffsets()
{
	g_dwReturn				= utilsFindPattern(g_dwHW_Base,g_dwHW_Len,(BYTE*)"\x57\x50\x51\x53\xE8","xxxxx");
	g_dwCL_ParseSound_Begin = utilsFindPattern(g_dwHW_Base,g_dwHW_Len,(BYTE*)"\x83\xEC\x68\x53\x55\x56\x57\x68\x00\x00\x00\x00\xE8","xxxxxxxx????x");
	g_dwCL_ParseSound_Call  = utilsFindPattern(g_dwHW_Base,g_dwHW_Len,(BYTE*)"\x50\x57\xE8\x00\x00\x00\x00\x83\xC4\x20\x5F\x5E\x5D\x5B\x83\xC4\x68\xC3","xxx????xxxxxxxxxxx");
	g_dwCL_ParseSound_End	= utilsFindPattern(g_dwHW_Base,g_dwHW_Len,(BYTE*)"\x50\x57\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x0C\x5F\x5E\x5D\x5B\x83\xC4\x68\xC3","xxx????x????xxxxxxxxxxx");
	g_dwSvc_Table			= utilsFindPattern(g_dwHW_Base,g_dwHW_Len,(BYTE*)"\x51\x53\x56\x57\x68\xFF\xFF\xFF\xFF\x33\xDB\xE8\xFF\xFF\xFF\xFF\x83\xC4\x04\x33\xF6\xBF","xxxxx????xxx????xxxxxx");
	g_dwTimeOfs				= utilsFindPattern(g_dwHW_Base,g_dwHW_Len,(BYTE*)"\x51\xDD\x05\x00\x00\x00\x00\xD9\x5C\x24\x00\xD9\x44\x24\x00\x59","xxx????xxxxxxxxx");

	if( !g_dwReturn )
		g_dwReturn = utilsFindPattern( g_dwHW_Base , g_dwHW_Len , (BYTE*)"\x83\xC4\x40\x8B\xF8","xxxxx");
	else
		g_dwReturn += 9;

	//correct values
	g_dwTimeOfs				+= 1;
	g_dwSvc_Table			+= 0x15;
	g_dwCL_ParseSound_End	+= 0x16;
	g_dwCL_ParseSound_Call	+= 2;
	g_dwCL_ParseSound_Call	-= g_dwCL_ParseSound_Begin;
	g_globalTime			= (double*)(*(DWORD*)(g_dwTimeOfs  + 2));

	//set locations
	pSvcDispatch			  = (svc_entry_s*)(*(DWORD*)(g_dwSvc_Table + 1 ) - 4 );

	
}

//////////////////////////////////////////////////////////////////////////
void engineDecrypt(byte * pOfs, int iLen)
{
	BYTE xor = 0x57;
	for (int i = 0x44; i < iLen; i++){
		BYTE newdata = pOfs[i]^xor;
		pOfs[i] = newdata;
		xor += newdata + 0x57;
	}
}

//////////////////////////////////////////////////////////////////////////
void engineEncrypt(BYTE *pOfs, int iLen)
{
	BYTE xor = 0x57;
	BYTE bDecoded;
	for( int i = 0x44; i < iLen; i++ )
	{
		bDecoded = pOfs[i] ^ xor;
		xor += pOfs[i] + 0x57;
		pOfs[i] = bDecoded;
	}
}


//////////////////////////////////////////////////////////////////////////
DWORD engineDecodeDLL(BYTE* pData, char **ppszDllName, DWORD *pdwExportTable, int iSize) 
{
	header_t * hdr = (header_t* )(pData + 0x44);
	segment_t* seg = (segment_t*)(pData + 0x5C);

	utilsDPrint("decoding hw.dll...");

	// retrieve hw.dll base & iLen
	engineDecrypt(pData, iSize);
	
	g_dwHW_Base = seg->baseaddress;
	g_dwHW_Len = seg->memorysize;
	
	engineEncrypt(pData, iSize);

	DWORD dwRetval = pHL_DecodeFunc(pData, ppszDllName, pdwExportTable, iSize);

	// retrieves all the offsets
	engineFindOffsets();
	utilsDPrint("system offsets retrieved successfully");

	// hook S_StartDynamicSound caller
	pSound = (sound_t)engineHookSvcEntry(g_dwCL_ParseSound_Begin, g_dwCL_ParseSound_End,
		g_dwCL_ParseSound_Call, (DWORD)S_StartDynamicSound,	6);

	return dwRetval;
}

//////////////////////////////////////////////////////////////////////////
__declspec(naked) void engineDecodeSteamDLL_stub()
{
	__asm{
	
		push	edi
		call	dword ptr ds:[g_dwCall]
		pop		ecx
		mov		ebx,eax
		mov		ecx,dword ptr ds:[g_dwPtr]
		mov		ecx,[ ecx ]
		push	esi
		push	edi
		push	ebx
		mov		eax,dword ptr ds:[ecx]
		call	dword ptr ds:[eax + 0x54]
		push	edi
		push	dword ptr ss:[esp + 0x1C]
		push	dword ptr ss:[esp + 0x1C]
		push	ebx
		call	engineDecodeDLL
		jmp		dword ptr ds:[g_dwJmp]
	}

}

//////////////////////////////////////////////////////////////////////////
__declspec(naked)void engineDecodeDLL_stub()
{
	__asm{
		mov		ecx,dword ptr ss:[esp + 0x48]
		mov		edx,dword ptr ss:[esp + 0x44]
		push	edi
		push	ecx
		push	edx
		push	ebx
		call	engineDecodeDLL
		jmp		dword ptr ds:[g_dwJmp]
	}
}


//////////////////////////////////////////////////////////////////////////
void enginePatchEngine()
{
	DWORD dwDecode;
	PIMAGE_DOS_HEADER     pDosHdr = (PIMAGE_DOS_HEADER)((DWORD)GetModuleHandle(0));
	PIMAGE_NT_HEADERS     pNtHdr  = (PIMAGE_NT_HEADERS)((BYTE*)(pDosHdr) + pDosHdr->e_lfanew);
	PIMAGE_SECTION_HEADER pSecHdr = (PIMAGE_SECTION_HEADER)( pNtHdr + 1 );

	utilsDPrint("searching for hl type...");

	for (int i=0; i < pNtHdr->FileHeader.NumberOfSections; i++){
		
		if((pSecHdr->Characteristics & IMAGE_SCN_CNT_CODE ) == IMAGE_SCN_CNT_CODE ){
			
			dwDecode = utilsFindPattern( (DWORD)(pDosHdr)+pSecHdr->VirtualAddress ,  pSecHdr->SizeOfRawData, 
				(BYTE*)"\x55\x8B\xEC\x51\x83\x65\xFC\x00\x53\x8B\x5D\x08\x57\x6A\x44\xB2\x57","xxxxxxxxxxxxxxxxx");
			
			if(!dwDecode){
				dwDecode = utilsFindPattern( (DWORD)(pDosHdr)+pSecHdr->VirtualAddress, pSecHdr->SizeOfRawData,
					(BYTE*)"\x51\x53\x55\x8B\x6C\x24\x10\x56\x8B\x74\x24\x20\x57","xxxxxxxxxxxxx");
			}
			
			if(dwDecode)
				break; 
		}
		pSecHdr++;
	}

	if(!dwDecode){
		utilsDPrint("search failed!");
		return;
	}

	if((dwDecode & 0x0FF00000) == 0x01400000 ){ //steam

		utilsDPrint("steam detected! patching...");
		
		g_dwJmp			= ( dwDecode-0x1D );
		g_dwCall		= utilsReconstructJMP(( dwDecode - 0x42 ));
		g_dwPtr			= *(DWORD*)( dwDecode - 0x38);

		g_dwFixaddr	= dwDecode - 0x43;
		
		memcpy((void*)g_FixValue, (void*)g_dwFixaddr, sizeof(g_FixValue));
		
		// not using splice, just stubs
		utilsCreateJMP(g_dwFixaddr, (DWORD)engineDecodeSteamDLL_stub);
		

	}else{

		utilsDPrint("non-steam detected! patching...");
		
		g_dwJmp		= dwDecode - 0x21;
		g_dwFixaddr	= dwDecode - 0x32;

		memcpy((void*)g_FixValue, (void*)g_dwFixaddr, sizeof(g_FixValue));
		
		// not using splice, just stubs
		utilsCreateJMP(g_dwFixaddr, (DWORD)engineDecodeDLL_stub);
	}

	pHL_DecodeFunc = (HL_DecodeFunc_t)(dwDecode);
}