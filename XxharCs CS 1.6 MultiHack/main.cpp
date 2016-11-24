#include <Windows.h>
#include "install.h"
#include "misc/utils.h"
#include "ModuleSecurity.h"



//////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:

		//HideModuleFromPEB(hinstDLL);
		//RemovePeHeader((DWORD)hinstDLL);

		utilsDPrint("Initializing...");
		InstallCheat();

		break;

	case DLL_PROCESS_DETACH:

		UninstallCheat();

		break;
	}

	return TRUE;
}