#include <Windows.h>
#include "opengl.h"

DWORD WINAPI HackThread(LPVOID unused)
{
	InstallGL();

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:

		CreateThread(0, 0, &HackThread, 0, 0, 0);

		break;

	case DLL_PROCESS_DETACH:

		//UninstallGL();

		break;
	}

	return TRUE;
}