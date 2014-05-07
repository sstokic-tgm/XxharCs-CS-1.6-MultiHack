#include <Windows.h>
#include "ModuleSecurity.h"

typedef struct _LDR_MODULE_DUMMY_
{
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
} LDR_MODULE, *PLDR_MODULE;

typedef struct _PEB_LDR_DATA_DUMMY_
{
	ULONG Length;
	UCHAR Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct PEB_DUMMY_
{
	PVOID Dummy1;
	PVOID Dummy2;
	PVOID Dummy3;
	PPEB_LDR_DATA LDR_Data;
} PEB_DUMMY, *PPEB_DUMMY;

PPEB_DUMMY __declspec(naked) GetPEB(void)
{
	_asm
	{
		mov eax, fs:[0x30];
		retn;
	}
}

void HideModuleFromPEB(HINSTANCE hInstance)
{
	PPEB_DUMMY PEB = GetPEB();

	PLIST_ENTRY FirstEntryInLoadOrder = (PLIST_ENTRY)&PEB->LDR_Data->InLoadOrderModuleList;
	PLIST_ENTRY FirstEntryInMemoryOrder = (PLIST_ENTRY)&PEB->LDR_Data->InMemoryOrderModuleList;
	PLIST_ENTRY FirstEntryInInitOrder = (PLIST_ENTRY)&PEB->LDR_Data->InInitializationOrderModuleList;

	for(PLIST_ENTRY TempEntry = FirstEntryInLoadOrder->Flink; TempEntry != FirstEntryInLoadOrder; TempEntry = TempEntry->Flink)
	{
		PLDR_MODULE TempModule = (PLDR_MODULE)((DWORD)TempEntry - 0 * sizeof(LIST_ENTRY));
		if(TempModule->BaseAddress == hInstance)
		{
			TempEntry->Blink->Flink = TempEntry->Flink;
			TempEntry->Flink->Blink = TempEntry->Blink;
			break;
		}
	}

	for(PLIST_ENTRY TempEntry = FirstEntryInMemoryOrder->Flink; TempEntry != FirstEntryInMemoryOrder; TempEntry = TempEntry->Flink)
	{
		PLDR_MODULE TempModule = (PLDR_MODULE)((DWORD)TempEntry - 1 * sizeof(LIST_ENTRY));
		if(TempModule->BaseAddress == hInstance)
		{
			TempEntry->Blink->Flink = TempEntry->Flink;
			TempEntry->Flink->Blink = TempEntry->Blink;
			break;
		}
	}

	for(PLIST_ENTRY TempEntry = FirstEntryInInitOrder->Flink; TempEntry != FirstEntryInInitOrder; TempEntry = TempEntry->Flink)
	{
		PLDR_MODULE TempModule = (PLDR_MODULE)((DWORD)TempEntry - 2 * sizeof(LIST_ENTRY));
		if(TempModule->BaseAddress == hInstance)
		{
			TempEntry->Blink->Flink = TempEntry->Flink;
			TempEntry->Flink->Blink = TempEntry->Blink;
			break;
		}
	}
}

void RemovePeHeader(DWORD ModuleBase)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ModuleBase;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);

	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return;

	if(pNTHeader->FileHeader.SizeOfOptionalHeader)
	{
		DWORD Protect;
		WORD Size = pNTHeader->FileHeader.SizeOfOptionalHeader;
		VirtualProtect((void*)ModuleBase, Size, PAGE_EXECUTE_READWRITE, &Protect);
		RtlZeroMemory((void*)ModuleBase, Size);
		VirtualProtect((void*)ModuleBase, Size, Protect, &Protect);
	}
}