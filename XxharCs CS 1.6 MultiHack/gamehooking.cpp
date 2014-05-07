#include <windows.h>
#include "gamehooking.h"
#include "NoSpread.h"


//////////////////////////////////////////////////////////////////////////
_CLIENT_* pClient;

BOOL bClientActive = FALSE;
BOOL bEngineActive = FALSE;

extern cl_enginefuncs_s* pEngfuncs;
extern struct cl_enginefuncs_s gEngfuncs;
extern engine_studio_api_s* pStudio;
extern engine_studio_api_t IEngineStudio;

extern pfnUserMsgHook TeamInfoOrg;
extern pfnUserMsgHook SetFOVOrg;
extern pfnUserMsgHook CurWeaponOrg;
extern pfnUserMsgHook ScoreAttribOrg;
extern pfnUserMsgHook HealthOrg;
extern pfnUserMsgHook BatteryOrg;
extern pfnUserMsgHook ScoreInfoOrg;
extern pfnUserMsgHook DeathMsgOrg;
extern pfnUserMsgHook SayTextOrg;
extern pfnUserMsgHook ResetHUDOrg;
extern pfnUserMsgHook TextMsgOrg;
extern pfnUserMsgHook DamageOrg;
extern pfnUserMsgHook AmmoXOrg;
extern pfnUserMsgHook WeaponListOrg;
extern pfnUserMsgHook MoneyOrg;

//////////////////////////////////////////////////////////////////////////
int	HookUserMsg (char *szMsgName, pfnUserMsgHook pfn)
{
#define REDIRECT_MESSAGE(name) \
		else if (!strcmp(szMsgName,#name)) \
	{ \
	name##Org = pfn; \
	return gEngfuncs.pfnHookUserMsg (szMsgName, ##name ); \
}

	int retval = gEngfuncs.pfnHookUserMsg (szMsgName, pfn);
	
	if(0){}
	REDIRECT_MESSAGE( TeamInfo    )
	REDIRECT_MESSAGE( CurWeapon   )
	REDIRECT_MESSAGE( ScoreAttrib )
	REDIRECT_MESSAGE( SetFOV      )
	REDIRECT_MESSAGE( Health	  )
	REDIRECT_MESSAGE( Battery     )
	REDIRECT_MESSAGE( ScoreInfo	  )
	REDIRECT_MESSAGE( DeathMsg    ) 
	REDIRECT_MESSAGE( SayText     )
	REDIRECT_MESSAGE( TextMsg     )
	REDIRECT_MESSAGE( ResetHUD    )
	REDIRECT_MESSAGE( Damage	  )
	REDIRECT_MESSAGE( AmmoX		  )
	REDIRECT_MESSAGE( Money		  )
	else if (!strcmp(szMsgName,"WeaponList")) // Because the Class is called like the Msg
	{
	WeaponListOrg = pfn;
	retval = gEngfuncs.pfnHookUserMsg(szMsgName, WeaponListMsg);
	}
	return retval;
}


//////////////////////////////////////////////////////////////////////////
CLIENT gClient = { NULL };
BOOL ActivateClient()
{
	// Copy client  to local structure
	memcpy( &gClient, (LPVOID)pClient, sizeof (CLIENT) );
	
	// Hook client  functions
	pClient->Initialize =					( INITIALIZE_FUNCTION)			&Initialize;
	pClient->HUD_Init =						( HUD_INIT_FUNCTION )			&HUD_Init;
	pClient->HUD_Frame =					( HUD_FRAME_FUNCTION )			&HUD_Frame;
	pClient->HUD_Redraw =					( HUD_REDRAW_FUNCTION )			&HUD_Redraw;
	pClient->HUD_PlayerMove =				( HUD_CLIENTMOVE_FUNCTION)		&HUD_PlayerMove;
	pClient->CL_CreateMove =				( HUD_CL_CREATEMOVE_FUNCTION )	&CL_CreateMove;
	pClient->V_CalcRefdef =					( HUD_V_CALCREFDEF_FUNCTION )	&PreV_CalcRefdef;
	pClient->HUD_AddEntity =				( HUD_ADDENTITY_FUNCTION )		&HUD_AddEntity;
	pClient->HUD_PostRunCmd =				( HUD_POSTRUNCMD_FUNCTION )		&HUD_PostRunCmd;
	pClient->HUD_Key_Event =				( HUD_KEY_EVENT_FUNCTION )		&HUD_Key_Event;
	pClient->HUD_UpdateClientData =			( HUD_UPDATECLIENTDATA_FUNCTION)&HUD_UpdateClientData;
	pClient->HUD_GetStudioModelInterface =  ( HUD_STUDIO_INTERFACE_FUNCTION) &HUD_GetStudioModelInterface;

	bClientActive = TRUE;
	return TRUE;
}



//==================================================================================
// Copy enginefuncs_s struct to local gEngfuncs and setup engine hooks
//==================================================================================
BOOL ActivateEngine()
{
	if(IsBadReadPtr((LPCVOID)pEngfuncs, sizeof DWORD)){
		return FALSE;
	}
	if( pEngfuncs->pfnHookUserMsg && pEngfuncs->pfnHookEvent ){

			memcpy( &gEngfuncs, pEngfuncs, sizeof( cl_enginefunc_t ) );
			if( pStudio->GetModelByIndex ){
				memcpy( &IEngineStudio, pStudio, sizeof( IEngineStudio ) );
			} else {
				return FALSE;
			}

			bEngineActive = TRUE;
			return TRUE;
	}
		
	return FALSE;
}
//==================================================================================