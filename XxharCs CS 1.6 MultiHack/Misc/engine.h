#ifndef __ENGINE_H__
#define __ENGINE_H__

typedef struct {
	DWORD unknown1;
	DWORD numsegments;
	DWORD initexpfunc;
	DWORD iataddr;
	DWORD dllmain;
	DWORD iathdr;
	DWORD imgbase;
	DWORD imgsize;
} header_t;

typedef struct
{ 
	DWORD baseaddress;	// +0 - The address the segment is allocated to
	DWORD memorysize;	// +4 - The Size that the memory needs to be allocated
	DWORD realsize;		// +8 - The Real size that the memory needs to be allocated
	DWORD fileoffset;	// +C - The offset from the start of the file to the start of the segment
	DWORD unknown1;		// +10 - Unknown
}segment_t; 

struct svc_entry_s
{
	DWORD dwId;			// +0 - id :)
	char  *szName;		// +4 - pointer to the name string
	DWORD pfnHandler;	// +C - the parser itself
};

typedef DWORD  (*HL_DecodeFunc_t)( BYTE* , char** , DWORD * , int );
typedef void   (*sound_t)(int,int,char*,float*,float,float,int,int);


void enginePatchEngine();

#endif