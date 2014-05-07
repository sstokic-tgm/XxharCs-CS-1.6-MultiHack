#include <Windows.h>
#include "splice.h"
#include "utils.h"



//////////////////////////////////////////////////////////////////////////
PSPLICE_ENTRY SpliceHookFunction(LPVOID function, LPVOID handler)
{
    if (!function || !handler) return NULL;

	LPVOID			trampoline;
	PSPLICE_ENTRY	psplice;

	DWORD			curr_summ = 0, dwlen = 0;

	trampoline = utilsVAlloc(20); // no more memory required for this

	while(curr_summ < JMP_SIZE){

		utilsGetInstructionLength((LPVOID)((DWORD)function + curr_summ), &dwlen);

		CopyMemory((PVOID)((DWORD)trampoline + curr_summ),
			(PVOID)((DWORD)function + curr_summ), dwlen);

		//
		// fixup jmps and calls
		//

		if (*(BYTE*)((DWORD)trampoline + curr_summ) == 0xe8
			||*(BYTE*)((DWORD)trampoline + curr_summ) == 0xe9){

			*(ULONG*)((DWORD)trampoline + curr_summ + 1)-=
				(ULONG)((DWORD)trampoline + curr_summ)-
				(ULONG)((DWORD)function + curr_summ);
		}

		USHORT w = MAKEWORD(*(BYTE*)((DWORD)trampoline + curr_summ +1),*(BYTE*)((DWORD)trampoline + curr_summ));
		if ((w&0xfff0) == 0xf80){ // jz/jnz/jb/jg etc found
		
			*(DWORD*)((DWORD)trampoline + curr_summ + 2)-=
				(DWORD)((DWORD)trampoline + curr_summ)-
				(DWORD)((DWORD)function + curr_summ);
		}
		curr_summ += dwlen;
	}

		
	utilsCreateJMP((DWORD)trampoline + curr_summ, (DWORD)function + curr_summ);
	utilsCreateJMP((DWORD)function, (DWORD)handler);

	psplice = (PSPLICE_ENTRY)utilsVAlloc(sizeof(SPLICE_ENTRY));

	psplice->repair_code_size = curr_summ;
	psplice->trampoline = trampoline;
	psplice->original_addr = function;
	
	return psplice;
}

//////////////////////////////////////////////////////////////////////////
void SpliceUnHookFunction(PSPLICE_ENTRY pse)
{
	if(!pse) return;

	DWORD prot;

	VirtualProtect(pse->original_addr, pse->repair_code_size, PAGE_EXECUTE_READWRITE, &prot);
	CopyMemory(pse->original_addr, pse->trampoline, pse->repair_code_size);
	VirtualProtect(pse->original_addr, pse->repair_code_size, prot, &prot);

	utilsVFree(pse->trampoline);
	utilsVFree(pse);
}



