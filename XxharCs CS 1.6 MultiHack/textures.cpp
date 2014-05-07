#include <windows.h>
#include <string>
#include <vector>
#include "gamehooking.h"
#include "players.h"
#include "perfectwall.h"


#define CTEXTURESMAX		512			// max number of textures loaded
#define CBTEXTURENAMEMAX	13			// only load first n chars of name

static int gcTextures = 0;
static char grgszTextureName[CTEXTURESMAX][CBTEXTURENAMEMAX];	
static char grgchTextureType[CTEXTURESMAX];


char FindTextureType( char *name )
{
	int left, right, pivot;
	int val;

	left = 0;
	right = gcTextures - 1;

	while ( left <= right )
	{
		pivot = ( left + right ) / 2;

		val = strnicmp( name, grgszTextureName[ pivot ], CBTEXTURENAMEMAX-1 );
		if ( val == 0 )
		{
			return grgchTextureType[ pivot ];
		}
		else if ( val > 0 )
		{
			left = pivot + 1;
		}
		else if ( val < 0 )
		{
			right = pivot - 1;
		}
	}

	return CHAR_TEX_CONCRETE;
}


char GetTextureType(pmtrace_t *tr, float *vecSrc, float *vecEnd)
{
	char chTextureType = CHAR_TEX_CONCRETE;
	int entity;
	char *pTextureName;
	char texname[ 64 ];
	char szbuffer[ 64 ];

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace(tr);

	chTextureType = 0;

	// Player
	if (entity >= 1 && entity <= gEngfuncs.GetMaxClients())
	{
		// hit body
		chTextureType = CHAR_TEX_FLESH;
	}
	else if (entity == 0)
	{
		// get texture from entity or world (world is ent(0))
		pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture(tr->ent, vecSrc, vecEnd);
		
		if (pTextureName)
		{
			strcpy( texname, pTextureName );
			pTextureName = texname;

			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pTextureName == '-' || *pTextureName == '+')
			{
				pTextureName += 2;
			}

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
			{
				pTextureName++;
			}
			
			// '}}'
			strcpy( szbuffer, pTextureName );
			szbuffer[ CBTEXTURENAMEMAX - 1 ] = 0;
				
			// get texture type
			chTextureType = FindTextureType( szbuffer );	
		}
	}

	return chTextureType;
}


void PM_SwapTextures( int i, int j )
{
	char chTemp;
	char szTemp[ CBTEXTURENAMEMAX ];

	strcpy( szTemp, grgszTextureName[ i ] );
	chTemp = grgchTextureType[ i ];
	
	strcpy( grgszTextureName[ i ], grgszTextureName[ j ] );
	grgchTextureType[ i ] = grgchTextureType[ j ];

	strcpy( grgszTextureName[ j ], szTemp );
	grgchTextureType[ j ] = chTemp;
}


void SortTextures( void )
{
	// Bubble sort, yuck, but this only occurs at startup and it's only 512 elements...
	//
	int i, j;

	for ( i = 0 ; i < gcTextures; i++ )
	{
		for ( j = i + 1; j < gcTextures; j++ )
		{
			if ( stricmp( grgszTextureName[ i ], grgszTextureName[ j ] ) > 0 )
			{
				// Swap
				//
				PM_SwapTextures( i, j );
			}
		}
	}
}


void InitTextureTypes(struct playermove_s *ppmove)
{
	char buffer[512];
	int i, j;
	byte *pMemFile;
	int fileSize, filePos;
	static qboolean bTextureTypeInit = false;

	if ( bTextureTypeInit )
		return;

	memset(&(grgszTextureName[0][0]), 0, CTEXTURESMAX * CBTEXTURENAMEMAX);
	memset(grgchTextureType, 0, CTEXTURESMAX);

	gcTextures = 0;
	memset(buffer, 0, 512);

	fileSize = ppmove->COM_FileSize("sound/materials.txt");
	pMemFile = ppmove->COM_LoadFile("sound/materials.txt", 5, NULL);
	if ( !pMemFile )
		return;

	filePos = 0;
	// for each line in the file...
	while (ppmove->memfgets( pMemFile, fileSize, &filePos, buffer, 511 ) != NULL && gcTextures < CTEXTURESMAX)
	{
		// skip whitespace
		i = 0;
		while(buffer[i] && isspace(buffer[i]))
			i++;
		
		if (!buffer[i])
			continue;

		// skip comment lines
		if (buffer[i] == '/' || !isalpha(buffer[i]))
			continue;

		// get texture type
		grgchTextureType[gcTextures] = toupper(buffer[i++]);

		// skip whitespace
		while(buffer[i] && isspace(buffer[i]))
			i++;
		
		if (!buffer[i])
			continue;

		// get sentence name
		j = i;
		while (buffer[j] && !isspace(buffer[j]))
			j++;

		if (!buffer[j])
			continue;

		// null-terminate name and save in sentences array
		j = min (j, CBTEXTURENAMEMAX-1+i);
		buffer[j] = 0;
		strcpy(&(grgszTextureName[gcTextures++][0]), &(buffer[i]));
	}

	// Must use engine to free since we are in a .dll
	ppmove->COM_FreeFile ( pMemFile );

	SortTextures();

	bTextureTypeInit = true;
}