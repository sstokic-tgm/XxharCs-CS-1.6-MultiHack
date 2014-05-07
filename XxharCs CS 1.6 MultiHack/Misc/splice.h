#ifndef __SPLICE_H__
#define __SPLICE_H__


//////////////////////////////////////////////////////////////////////////
// splice internals

#define JMP_SIZE	5

typedef struct _splice_entry{
	
	LPVOID	original_addr;
	LPVOID	trampoline;
	DWORD	repair_code_size; // without JMP_SIZE

}SPLICE_ENTRY, *PSPLICE_ENTRY;

//////////////////////////////////////////////////////////////////////////
// prototypes
PSPLICE_ENTRY	SpliceHookFunction(LPVOID function, LPVOID handler);
void			SpliceUnHookFunction(PSPLICE_ENTRY pse);

#endif