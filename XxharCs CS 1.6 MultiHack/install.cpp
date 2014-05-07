#include <windows.h>
#include "install.h"
#include "gamehooking.h"
#include "misc/engine.h"
#include "misc/splice.h"
#include "opengl.h"


//////////////////////////////////////////////////////////////////////////
// global offsets
extern cl_enginefuncs_s* pEngfuncs;
extern engine_studio_api_s* pStudio;
extern _CLIENT_* pClient;
extern double* globalTime;

//////////////////////////////////////////////////////////////////////////

int XREAD_LONG( unsigned char* gpBuf )
{
	return gpBuf[0] + (gpBuf[1] << 8) + (gpBuf[2] << 16) + (gpBuf[3] << 24);
}

DWORD Base, Size = 0;

void InitOffsets()
{
	DWORD HlBase = 0;
	DWORD HwBase = 0;

	HlBase = (DWORD)GetModuleHandleA(NULL);
	HwBase = (DWORD)GetModuleHandleA((LPCSTR)"hw.dll");

	if(HwBase == 0) 
	{	// 47 protocol
		Base = HlBase;
		Size = 0x02116000;
	} 
	else
	{
		Base = HwBase;
		Size = 0x0122A000;
	}

	DWORD enginefunctions = utilsFindPattern(Base, Size, (BYTE*)"invalid sound %i\x0A", "xxxxxxxxxxxxxxxxx")+24;
	pEngfuncs = (cl_enginefuncs_s*)enginefunctions;

	if( IsBadReadPtr((LPCVOID)pEngfuncs->pfnClientCmd, sizeof DWORD) )
	{
		MessageBoxA(0, "BadReadPtr: pEngfuncs->pfnClientCmd", "Fatal Error", MB_OK|MB_ICONERROR);
		ExitProcess(0);
	}

	unsigned char* PushEngstudioPointer = (unsigned char*)utilsFindPattern(Base, Size, 
		(BYTE*)"\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x6A\x01\xFF\xD0\x83\xC4\x0C\x85\xC0\x75\x12\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04\xE9\x00\x00\x00\x00\xC3",
		"x????x????xxxxxxxxxxxx????x????xxxx????x");
	if( !IsBadReadPtr((LPCVOID)PushEngstudioPointer, sizeof DWORD) )
	{
		pStudio = (engine_studio_api_s*)XREAD_LONG(PushEngstudioPointer+1);
		if( IsBadReadPtr((LPCVOID)pStudio, sizeof DWORD) ) 
		{
			MessageBoxA(0, "BadReadPtr: pStudio", "Fatal Error", MB_OK|MB_ICONWARNING);
			ExitProcess(0);
		}
	} else 
	{
		MessageBoxA(0, "BadReadPtr: PushEngstudioPointer", "Fatal Error", MB_OK|MB_ICONWARNING);
		ExitProcess(0);
	}

	__try { pClient = (_CLIENT_*)(*(DWORD*)(utilsFindPattern((DWORD)Base, Size, (BYTE*)"\x8B\x44\x24\x04\x6A\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xC7\x05", "xxxxxxx????x????xxx")+7)); } __except ( EXCEPTION_CONTINUE_EXECUTION ) { pClient = 0; } 
	if ( IsBadReadPtr((void*)pClient, sizeof(_CLIENT_)) )
	{
		MessageBoxA(0, "BadReadPtr: pClient", "Fatal Error", MB_OK|MB_ICONWARNING);
		ExitProcess(0);
	}
}



//////////////////////////////////////////////////////////////////////////
PSPLICE_ENTRY xQPC;
extern BOOL bClientActive;
extern BOOL bEngineActive;
BOOL WINAPI xQueryPerformanceCounter(LARGE_INTEGER* pLI)
{
	if (!bClientActive) ActivateClient();
	if (!bEngineActive)	ActivateEngine();

	return (((BOOL(WINAPI*)(LARGE_INTEGER*))xQPC->trampoline)(pLI));
}

//////////////////////////////////////////////////////////////////////////
void InstallCheat()
{
	InitOffsets();
	enginePatchEngine();
	InstallGL();
	xQPC = SpliceHookFunction(QueryPerformanceCounter, xQueryPerformanceCounter); 
}

//////////////////////////////////////////////////////////////////////////
void UninstallCheat()
{
	UninstallGL();
}

