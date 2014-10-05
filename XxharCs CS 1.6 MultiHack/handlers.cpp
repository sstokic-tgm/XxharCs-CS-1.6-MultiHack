#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <tchar.h>
#include <fstream>

#include "gamehooking.h"
#include "players.h"
#include "weaponlist.h"
#include "playeritems.h"
#include "cvars.h"
#include "NoRecoil.h"
#include "NoSpread.h"
#include "Aimbot.h"
#include "eventhook.h"


//////////////////////////////////////////////////////////////////////////
// original variables
//////////////////////////////////////////////////////////////////////////
CCvars cvars;
extern bool bAim;
static bool bunnyhop = false;

cl_enginefuncs_s gEngfuncs		= { NULL };  
cl_enginefuncs_s g_Engine;
cl_enginefuncs_s* g_pEngine;
cl_enginefuncs_s* pEngfuncs;
engine_studio_api_s* pStudio;


double* globalTime;
engine_studio_api_t IEngineStudio		= { NULL };
extern CLIENT gClient;

typedef struct cl_clientfuncs_s
{
	int ( *Initialize ) ( cl_enginefunc_t *pEnginefuncs, int iVersion );
	int ( *HUD_Init ) ( void );
	int ( *HUD_VidInit ) ( void );
	void ( *HUD_Redraw ) ( float time, int intermission );
	int ( *HUD_UpdateClientData ) ( client_data_t *pcldata, float flTime );
	int ( *HUD_Reset ) ( void );
	void ( *HUD_PlayerMove ) ( struct playermove_s *ppmove, int server );
	void ( *HUD_PlayerMoveInit ) ( struct playermove_s *ppmove );
	char ( *HUD_PlayerMoveTexture ) ( char *name );
	void ( *IN_ActivateMouse ) ( void );
	void ( *IN_DeactivateMouse ) ( void );
	void ( *IN_MouseEvent ) ( int mstate );
	void ( *IN_ClearStates ) ( void );
	void ( *IN_Accumulate ) ( void );
	void ( *CL_CreateMove ) ( float frametime, struct usercmd_s *cmd, int active );
	int ( *CL_IsThirdPerson ) ( void );
	void ( *CL_CameraOffset ) ( float *ofs );
	struct kbutton_s *( *KB_Find ) ( const char *name );
	void ( *CAM_Think ) ( void );
	void ( *V_CalcRefdef ) ( struct ref_params_s *pparams );
	int ( *HUD_AddEntity ) ( int type, struct cl_entity_s *ent, const char *modelname );
	void ( *HUD_CreateEntities ) ( void );
	void ( *HUD_DrawNormalTriangles ) ( void );
	void ( *HUD_DrawTransparentTriangles ) ( void );
	void ( *HUD_StudioEvent ) ( const struct mstudioevent_s *event, const struct cl_entity_s *entity );
	void ( *HUD_PostRunCmd ) ( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed );
	void ( *HUD_Shutdown ) ( void );
	void ( *HUD_TxferLocalOverrides ) ( struct entity_state_s *state, const struct clientdata_s *client );
	void ( *HUD_ProcessPlayerState ) ( struct entity_state_s *dst, const struct entity_state_s *src );
	void ( *HUD_TxferPredictionData ) ( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd );
	void ( *Demo_ReadBuffer ) ( int size, unsigned char *buffer );
	int ( *HUD_ConnectionlessPacket ) ( struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
	int ( *HUD_GetHullBounds ) ( int hullnumber, float *mins, float *maxs );
	void ( *HUD_Frame ) ( double time );
	int ( *HUD_Key_Event ) ( int down, int keynum, const char *pszCurrentBinding );
	void ( *HUD_TempEntUpdate ) ( double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int ( *Callback_AddVisibleEntity )( struct cl_entity_s *pEntity ), void ( *Callback_TempEntPlaySound )( struct tempent_s *pTemp, float damp ) );
	struct cl_entity_s *( *HUD_GetUserEntity ) ( int index );
	int ( *HUD_VoiceStatus ) ( int entindex, qboolean bTalking );
	int ( *HUD_DirectorMessage ) ( unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags );
	int ( *HUD_GetStudioModelInterface ) ( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio );
	void ( *HUD_CHATINPUTPOSITION_FUNCTION ) ( int *x, int *y );
	int ( *HUD_GETPLAYERTEAM_FUNCTION ) ( int iplayer );
	void ( *CLIENTFACTORY ) ( void );
} cl_clientfunc_t;

extern cl_clientfunc_t *g_pClient;
extern cl_clientfunc_t g_Client;
extern cl_enginefunc_t *g_pEngine;
extern engine_studio_api_t *g_pStudio;
extern engine_studio_api_t g_Studio;
extern SCREENINFO g_Screen;
extern char g_szHackDir[256];


cl_clientfunc_t *g_pClient = NULL;
cl_clientfunc_t g_Client;
//cl_enginefunc_t *g_pEngine = NULL;
//cl_enginefunc_t g_Engine;
engine_studio_api_t *g_pStudio = NULL;
engine_studio_api_t g_Studio;

SCREENINFO g_Screen;

pfnUserMsgHook TeamInfoOrg=NULL;
pfnUserMsgHook SetFOVOrg=NULL;
pfnUserMsgHook CurWeaponOrg=NULL;
pfnUserMsgHook ScoreAttribOrg=NULL;
pfnUserMsgHook HealthOrg=NULL;
pfnUserMsgHook BatteryOrg=NULL;
pfnUserMsgHook ScoreInfoOrg=NULL;
pfnUserMsgHook DeathMsgOrg=NULL;
pfnUserMsgHook SayTextOrg=NULL;
pfnUserMsgHook ResetHUDOrg=NULL;
pfnUserMsgHook TextMsgOrg=NULL;
pfnUserMsgHook DamageOrg=NULL;
pfnUserMsgHook AmmoXOrg=NULL;
pfnUserMsgHook WeaponListOrg=NULL;
pfnUserMsgHook MoneyOrg=NULL;

extern float fCurrentFOV;
extern int	displayCenterX;
extern int	displayCenterY;
extern bool oglSubtractive;


//////////////////////////////////////////////////////////////////////////
// HUD_Init Client Function
//////////////////////////////////////////////////////////////////////////
void HUD_Init ( void )
{
	gClient.HUD_Init();

}

int Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	return gClient.Initialize(pEnginefuncs, iVersion);
}

void HookEngine( void )
{
	memcpy( &g_Engine, (LPVOID)g_pEngine, sizeof( cl_enginefunc_t ) );
	g_pEngine->pfnHookUserMsg = HookUserMsg;
}

void HookStudio( void )
{
	memcpy( &g_Studio, (LPVOID)g_pStudio, sizeof( engine_studio_api_t ) );
}

static bool Init = false;

void DrawConString (int x, int y, int r, int g, int b, const char *fmt, ... )
{
	va_list va_alist;
	char buf[256];

	va_start (va_alist, fmt);
	_vsnprintf (buf, sizeof(buf), fmt, va_alist);
	va_end (va_alist);

	int length, height;

	y += 4;
	gEngfuncs.pfnDrawConsoleStringLen( buf, &length, &height );
	gEngfuncs.pfnDrawSetTextColor(r/255.0f, g/255.0f, b/255.0f);
	gEngfuncs.pfnDrawConsoleString(x,y,buf);
}

void InitHack()
{
	g_Screen.iSize = sizeof( SCREENINFO );
	gEngfuncs.pfnGetScreenInfo( &g_Screen );

	char *HackPrint = "\nXxharCs MultiHack has been loaded\nby XxharCs\n\nCS 1.6 Version:\n-------------------\n";
	gEngfuncs.pfnConsolePrint(HackPrint);
	gEngfuncs.pfnClientCmd("version");
	gEngfuncs.pfnClientCmd("toggleconsole");
}

void HUD_Frame( double time )
{
	if(!Init)
	{
		InitHack();
		Init = true;
	}
	gClient.HUD_Frame( time );
}

int HUD_GetStudioModelInterface ( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio )
{
	

	return gClient.HUD_GetStudioModelInterface(version, ppinterface, pstudio);
}

//////////////////////////////////////////////////////////////////////////
// HUD_Redraw Client Function
//////////////////////////////////////////////////////////////////////////
/*
int GetTeam (char * model)
{
	// Check all terror team models
	if (strstr (model, "arctic") ||
		strstr (model, "guerilla") ||
		strstr (model, "leet") ||
		strstr (model, "militia") ||
		strstr (model, "terror"))
			return 1;

	// Check all counter team models
	else if (strstr (model, "gign") ||
		strstr (model, "gsg9") ||
		strstr (model, "sas") ||
		strstr (model, "urban") ||
		strstr (model, "spetsnaz") ||
		strstr (model, "vip"))
			return 2;

	// No Team / Spec / ...
	return 0;
} */

/*
void DrawBox(int x, int y, int r, int g, int b, int a, int iRadius, int iThickness)
{
	int iRad = iRadius / 2;
	// top
	g_pEngine->pfnFillRGBA(x - iRad, y - iRad, iRadius, iThickness, r, g, b, a);
 
	// bottom
	g_pEngine->pfnFillRGBA(x - iRad, y + iRad, iRadius + (iThickness / 2), iThickness, r, g, b, a);
 
	// left
	g_pEngine->pfnFillRGBA(x - iRad, y - iRad, iThickness, iRadius, r, g, b, a);
 
	// right
	g_pEngine->pfnFillRGBA(x + iRad, y - iRad, iThickness, iRadius, r, g, b, a);
}
/*
void DoEspAim (void)
{
	cl_entity_t *pEnt = NULL, *pLocal = gEngfuncs.GetLocalPlayer();
	hud_player_info_t pInfo;
	
	gEngfuncs.pfnGetPlayerInfo(pLocal-> index, &pInfo);
	
	int iOwnTeam = GetTeam(pInfo.model);  

	float fScreenPos[10];
	float fDistFromCenter[3];
	float fCenterDistance;

	
	int iScreenCenterX = gEngfuncs.GetWindowCenterX();
	int iScreenCenterY = gEngfuncs.GetWindowCenterY();

	
	cl_entity_t *pNearestEnt = NULL;
	float fNearest =-1.0f;
	float fNearestAimTarget[3] = {0,0,0}; 

	for (int i = 0; i <33; i++)
	{
		
		if (pLocal->index == i) continue;
		
		
		pEnt = gEngfuncs.GetEntityByIndex(i);
		if (!isValidEnt(pEnt)) continue;

		
		gEngfuncs.pfnGetPlayerInfo(i, &pInfo);
		int iTeam = GetTeam(pInfo.model); 
		pEnt->origin.z -= -20; 
		if (CalcScreen(pEnt->origin, fScreenPos))
        {
			
			fDistFromCenter[0] = iScreenCenterX - fScreenPos[0];
			fDistFromCenter[1] = iScreenCenterY - fScreenPos[1];
 
			
			fCenterDistance = Square(fDistFromCenter[0], 2) + Square(fDistFromCenter[1], 2); 
			if (iTeam == 1) // Team = Terror = Red
			{
				DrawBox(fScreenPos[0], fScreenPos[1], 255, 10, 10, 255, 25, 2);
			}
			else if (iTeam == 2) // Team = CT = Blue
			{
				DrawBox(fScreenPos[0], fScreenPos[1], 10, 10, 255, 255, 25, 2);
			}
			else 
			{
				continue;
			} 
			if (iTeam == iOwnTeam) continue; 

			if (fNearest <0.0f || fCenterDistance < fNearest)
			{
				
				pNearestEnt = pEnt;
				
				fNearest = fCenterDistance;
				
				fNearestAimTarget[0] = fScreenPos[0];
				fNearestAimTarget[1] = fScreenPos[1];
			}
		}
	} 
	if (pNearestEnt != NULL && fNearestAimTarget[0] > 0 && fNearestAimTarget[1] > 0)
	{
		
		#define AIM_SMOOTH 3

		if (GetAsyncKeyState (VK_LBUTTON))
		{
			
			float x = fNearestAimTarget [0] - iScreenCenterX;
			float y = fNearestAimTarget [1] - iScreenCenterY;

			x /= AIM_SMOOTH;
			y /= AIM_SMOOTH;
			
			fNearestAimTarget [0] = iScreenCenterX + x;
			fNearestAimTarget [1] = iScreenCenterY + y;

			
			SetCursorPos((int)fNearestAimTarget[0], (int)fNearestAimTarget[1]);
			
		}
	} 
}

/*
void Aimbot(int ax)
{
	float vec[2];
	if( !CalcScreen(vPlayers[ax].origin(), vec)) { return; }
	DrawConString(0, 550, 0, 255, 0, "vec: %f %f", vec[1],vec[2]);
}*/


void HUD_Redraw ( float x, int y )
{
	InitVisuals();
	//DoEspAim();
	
	
	/*Aimbot.FindTarget();
	Aimbot.DrawAimSpot();
	for (int ax=0;ax<MAX_VPLAYERS;ax++) if (vPlayers[ax].isUpdated() ) { drawPlayerEsp(ax); }
	for(int i=0;i<MAX_VPLAYERS;i++) 
		if(!bIsEntValid(vPlayers[i].getEnt(), i)) vPlayers[i].updateClear();*/

	/*
	cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();
	hud_player_info_t pInfo;
	gEngfuncs.pfnGetPlayerInfo(pLocal-> index, &pInfo);
	int iOwnTeam = GetTeam(pInfo.model); 
	
	char data[128];

	for (int ax=0;ax<MAX_VPLAYERS;ax++){
		gEngfuncs.pfnGetPlayerInfo(ax, &pInfo);
		int iTeam = GetTeam(pInfo.model); 

		

		if (vPlayers[ax].isUpdated() && vPlayers[ax].isAlive()){
			
			if(iOwnTeam == iTeam)
			{
				sprintf(data, "[%s] ist in meinem team!", vPlayers[ax].getEnt()->model->name);
				gEngfuncs.pfnDrawConsoleString(10, 10+ax*20, data);
			}
			else
			{
				sprintf(data, "[%s] ist nicht in meinem team!", vPlayers[ax].getEnt()->model->name);
				gEngfuncs.pfnDrawConsoleString(10, 10+ax*20, data);
			}
		}
		 
		if(!bIsEntValid(vPlayers[ax].getEnt(), ax))
			vPlayers[ax].updateClear();
	}
	
	*/

	/*// esp
	for (int ax = 0; ax < MAX_VPLAYERS; ax++){
		if (vPlayers[ax].isUpdated() && vPlayers[ax].isAlive())
		{
			//drawPlayerEsp(ax);
		}
		 
		if(!bIsEntValid(vPlayers[ax].getEnt(), ax))
			vPlayers[ax].updateClear();
	}*/

	gClient.HUD_Redraw(x, y);
}

//////////////////////////////////////////////////////////////////////////
// HUD_PlayerMove Client Function
//////////////////////////////////////////////////////////////////////////
void HUD_PlayerMove ( struct playermove_s *ppmove, qboolean server )
{
	me.pmMoveType = ppmove->movetype;
	VectorCopy(ppmove->velocity,me.pmVelocity);

	// copy origin+angles
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(me.pmEyePos);	
	me.pmEyePos[0] += ppmove->origin[0];
	me.pmEyePos[1] += ppmove->origin[1];
	me.pmEyePos[2] += ppmove->origin[2];
	me.pmFlags = ppmove->flags;

	VectorCopy(ppmove->angles,me.viewAngles);

	gClient.HUD_PlayerMove(ppmove, server);
}

//////////////////////////////////////////////////////////////////////////
// CL_CreateMove Client Function
//////////////////////////////////////////////////////////////////////////
void CL_CreateMove ( float frametime, struct usercmd_s *cmd, int active )
{
	gClient.CL_CreateMove(frametime, cmd, active);

	ApplyNoRecoil(frametime, me.punchangle, cmd->viewangles);
	

	//nospread
	if (cvars.nospread && me.alive)
	{
		float offset[3];
		gNoSpread.GetSpreadOffset(me.spread.random_seed, 1, cmd->viewangles, me.pmVelocity, offset);
		cmd->viewangles[0] += offset[0];
		cmd->viewangles[1] += offset[1];
		cmd->viewangles[2] += offset[2];
	}

	if(cvars.bunnyhop) 
	{
		me.DoBunnyHop(cmd);
	}
}

//////////////////////////////////////////////////////////////////////////
// Called before V_CalcRefdef Client Function
//////////////////////////////////////////////////////////////////////////
void PreV_CalcRefdef ( struct ref_params_s *pparams )
{
	if(cvars.norecoil)
	{
		VectorCopy(pparams->punchangle, me.punchangle);
		//only for visual NoRecoil:
		VectorClear(pparams->punchangle);

		if(!me.alive)
		{
			VectorCopy(pparams->vieworg,me.pmEyePos);
		}
	}
	gClient.V_CalcRefdef(pparams);
}
/*
//==================================================================================
// Called after V_CalcRefdef Client Function
//==================================================================================
void PostV_CalcRefdef ( struct ref_params_s *pparams )
{
	if( pparams->nextView == 0 )
	{
		// Primary Screen, below all other Viewports Sent
		// update vectors for CalcScreen
		VectorCopy(pparams->viewangles,mainViewAngles);
		VectorCopy(pparams->vieworg,mainViewOrigin);	
		if ( me.alive )          
		if ( bAim )          
		if ( me.iClip )           
		if ( Aimbot.HasTarget() )
		{
			// auto aim	
			Aimbot.CalculateAimingView();
			VectorCopy(Aimbot.aim_viewangles, pparams->viewangles);	
			Aimbot.active = true;
		}
	}
}*/

//////////////////////////////////////////////////////////////////////////
// HUD_AddEntity Client Function
//////////////////////////////////////////////////////////////////////////
int HUD_AddEntity ( int type, struct cl_entity_s *ent, const char *modelname )
{
	int AddEntResult;

	UpdateMe();
	AddEntResult=1;
	if(0){}
	else if( isValidEnt(ent) ) 
	{
		if(me.alive)
		{
			gEngfuncs.CL_CreateVisibleEntity(ET_PLAYER, ent);
			AddEntResult = 0;
		}
		vPlayers[ent->index].updateAddEntity(ent->origin); 
		vPlayers[ent->index].setAlive();
		vPlayers[ent->index].updateEntInfo();
		ent->curstate.rendermode = 0;
		ent->curstate.renderfx = 0;
		playerCalcExtraData(ent->index, ent);
	}
	if(ent->index == me.ent->index) 
	{ 
		int px = ent->index;

	}
	if(me.ent->curstate.iuser1 == 4 && me.ent->curstate.iuser2 == ent->index)
		AddEntResult=0;

	gClient.HUD_AddEntity(type, ent, modelname);

	return AddEntResult;
}

//////////////////////////////////////////////////////////////////////////
// HUD_PostRunCmd Client Function
//////////////////////////////////////////////////////////////////////////
void HUD_PostRunCmd ( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	gClient.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
	gNoSpread.HUD_PostRunCmd(from,to,cmd,runfuncs,time,random_seed);
}

//////////////////////////////////////////////////////////////////////////
// HUD_UpdateClientData Client Function
//////////////////////////////////////////////////////////////////////////
void HUD_UpdateClientData(client_data_t *cdata, float flTime)
{
	
	for (int i = 0; i < MAX_VPLAYERS ;i++){
		if(!bIsEntValid(vPlayers[i].getEnt(),i))
			vPlayers[i].updateClear();
	}
	WeaponListUpdate(cdata->iWeaponBits);

	gClient.HUD_UpdateClientData(cdata, flTime);
}

//////////////////////////////////////////////////////////////////////////
// HUD_Key_Event Client Function
//////////////////////////////////////////////////////////////////////////
int HUD_Key_Event ( int eventcode, int keynum, const char *pszCurrentBinding )
{
	//DoHLHAiming(eventcode);

	return gClient.HUD_Key_Event(eventcode, keynum, pszCurrentBinding);
}

//////////////////////////////////////////////////////////////////////////
// SPR_Load Engine function
//////////////////////////////////////////////////////////////////////////
HCSPRITE SPR_Load ( const char *szPicName )
{
	return gEngfuncs.pfnSPR_Load( szPicName );
}

//////////////////////////////////////////////////////////////////////////
// SPR_Set Engine Function
//////////////////////////////////////////////////////////////////////////
void SPR_Set ( HCSPRITE hPic, int r, int g, int b )
{
	return gEngfuncs.pfnSPR_Set( hPic, r, g, b );
}

//////////////////////////////////////////////////////////////////////////
// SPR_Draw Engine Function
//////////////////////////////////////////////////////////////////////////
void SPR_Draw ( int frame, int x, int y, const struct rect_s *prc )
{
	return gEngfuncs.pfnSPR_Draw( frame, x, y, prc );
}

//////////////////////////////////////////////////////////////////////////
// SPR_DrawHoles Engine Function
//////////////////////////////////////////////////////////////////////////
void SPR_DrawHoles ( int frame, int x, int y, const struct rect_s *prc )
{
	return gEngfuncs.pfnSPR_DrawHoles( frame, x, y, prc );
}

//////////////////////////////////////////////////////////////////////////
// SPR_DrawAdditive Engine Function
//////////////////////////////////////////////////////////////////////////
void SPR_DrawAdditive ( int frame, int x, int y, const struct rect_s *prc )
{
	return gEngfuncs.pfnSPR_DrawAdditive( frame, x, y, prc );
}


//////////////////////////////////////////////////////////////////////////
// DrawCharacter Engine Function
//////////////////////////////////////////////////////////////////////////
int DrawCharacter ( int x, int y, int number, int r, int g, int b )
{
	return gEngfuncs.pfnDrawCharacter( x, y, number, r, g, b );
}

//////////////////////////////////////////////////////////////////////////
// DrawConsoleString Engine Function
//////////////////////////////////////////////////////////////////////////
int DrawConsoleString ( int x, int y, char *string )
{
	return gEngfuncs.pfnDrawConsoleString( x, y, string );
}

//////////////////////////////////////////////////////////////////////////
// FillRGBA Engine Function
//////////////////////////////////////////////////////////////////////////
void FillRGBA ( int x, int y, int width, int height, int r, int g, int b, int a )
{
	return gEngfuncs.pfnFillRGBA( x, y, width, height, r, g, b, a );
}


//////////////////////////////////////////////////////////////////////////
int TeamInfo (const char *pszName, int iSize, void *pbuf)
{
	UpdateMe();
	BEGIN_READ(pbuf,iSize);
	int px = READ_BYTE();
	char * teamtext = READ_STRING();
	const char * STR_TERROR = "TERRORIST"; 
	const char * STR_CT = "CT"; 
	const char * STR_UNASSIGNED = "UNASSIGNED"; 
	const char * STR_SPECTATOR = "SPECTATOR"; 
	if  (!strcmpi (teamtext, STR_TERROR)) vPlayers[px].team = 1; 
	else if  (!strcmpi (teamtext, STR_CT)) vPlayers[px].team = 2; 
	else if  (!strcmpi (teamtext, STR_UNASSIGNED)) vPlayers[px].team = 0; 
	else if  (!strcmpi (teamtext, STR_SPECTATOR)) vPlayers[px].team = 0; 
	else { 
		vPlayers[px].team = -1; 
	} 

	if(px == gEngfuncs.GetLocalPlayer()->index)
	{
		if  (!strcmpi (teamtext, STR_TERROR)) me.team = 1; 
		else if  (!strcmpi (teamtext, STR_CT)) me.team = 2; 
		else if  (!strcmpi (teamtext, STR_UNASSIGNED)) me.team = 0; 
		else if  (!strcmpi (teamtext, STR_SPECTATOR)) me.team = 0; 
		else { 
			me.team = -1; 
		} 
	}
	
	return (*TeamInfoOrg)(pszName, iSize, pbuf);
}

//////////////////////////////////////////////////////////////////////////
int CurWeapon(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iState = READ_BYTE();
	int iID    = READ_CHAR();
	int iClip  = READ_CHAR();
	
	if (iState)
		me.iClip = iClip;
		
	return (*CurWeaponOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int ScoreAttrib(const char *pszName, int iSize, void *pbuf )
{
	UpdateMe();
	
	BEGIN_READ(pbuf, iSize);
	
	int idx  = READ_BYTE();
	int info = READ_BYTE();
	
	if(idx==me.ent->index)
		me.alive = ((info&1)==0);
	
	return (*ScoreAttribOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int SetFOV(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	me.iFOV = READ_BYTE();
	
	if(!me.iFOV){
		me.iFOV=90;
	}

	if(me.iFOV==90){
		me.inZoomMode=false;
	}
	else{
		 me.inZoomMode=true;
	}
	
	fCurrentFOV = me.iFOV;
	
	return (*SetFOVOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int Health(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	me.iHealth = READ_BYTE();

	return (*HealthOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int Battery(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	me.iArmor = READ_BYTE();
	return (*BatteryOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int ScoreInfo(const char *pszName, int iSize, void *pbuf)
{
	return (*ScoreInfoOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	UpdateMe();
	BEGIN_READ( pbuf, iSize );
	int killer = READ_BYTE();
	int victim = READ_BYTE();
	int headshot = READ_BYTE();
	char* weaponName = READ_STRING();
	if( killer==me.ent->index && headshot)
		me.iHs++;
	if( killer==me.ent->index && victim != me.ent->index )
		me.iKills++;
	if( victim==me.ent->index )
		me.iDeaths++;
	vPlayers[victim].setDead();
	vPlayers[victim].updateClear();

	return (*DeathMsgOrg)(pszName,iSize,pbuf);

}

//////////////////////////////////////////////////////////////////////////
int SayText(const char *pszName, int iSize, void *pbuf)
{
	return (*SayTextOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int TextMsg(const char *pszName, int iSize, void *pbuf)
{
	return (*TextMsgOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////

int ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	AtRoundStart();

	return (*ResetHUDOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int Damage(const char *pszName, int iSize, void *pbuf )
{
	return (*DamageOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int AmmoX(const char *pszName, int iSize, void *pbuf )
{
	return (*AmmoXOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int WeaponListMsg(const char *pszName, int iSize, void *pbuf )
{
	return (*WeaponListOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
int Money(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	me.iMoney = READ_SHORT();
	return (*MoneyOrg)(pszName,iSize,pbuf);
}

//////////////////////////////////////////////////////////////////////////
