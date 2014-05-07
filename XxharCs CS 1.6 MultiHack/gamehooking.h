#ifndef __GAMEHOOKING_H__
#define __GAMEHOOKING_H__

#pragma warning(disable:4996)

#include <Mmsystem.h>
#include <vector>
#include "engine/util_vector.h"
#include "engine/wrect.h"
#include "engine/cl_dll.h"
#include "engine/cdll_int.h"
#include "engine/const.h"
#include "engine/progdefs.h"
#include "engine/eiface.h"
#include "engine/edict.h"
#include "engine/studio_event.h"
#include "engine/entity_types.h"
#include "engine/r_efx.h"
#include "engine/pmtrace.h"
#include "engine/ref_params.h"
#include "engine/screenfade.h"
#include "engine/event_api.h"
#include "engine/com_model.h"
#include "misc/parsemsg.h"
#include "misc/sprites.h"
#include "misc/r_studioint.h"
#include "engine/triangleapi.h"
#include "engine/pm_defs.h"
#include "engine/studio.h"
#include "engine/cl_entity.h"
#include "misc/utils.h"
#include "Engine/cdll_dll.h"


#define M_PI 3.14159265358979323846
#define POW(x) ((x)*(x))
#define VectorLengthSquared(v) ((v)[0]*(v)[0]+(v)[1]*(v)[1]+(v)[2]*(v)[2])
#define VectorDistance(a,b) sqrt(POW((a)[0]-(b)[0])+POW((a)[1]-(b)[1])+POW((a)[2]-(b)[2]))
#define VectorLength(a) sqrt(POW((a)[0])+POW((a)[1])+POW((a)[2]))
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorMul(vec,num,res){(res)[0]=(vec)[0]*(num);(res)[1]=(vec)[1]*(num);(res)[2]=(vec)[2]*(num);}
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define Square(a) ((a)*(a))
#define VectorScale(v, s, o)    ((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))

#define WEAPONLIST_P228			1
#define	WEAPONLIST_UNKNOWN1		2
#define	WEAPONLIST_SCOUT		3
#define	WEAPONLIST_HEGRENADE	4
#define	WEAPONLIST_XM1014		5
#define	WEAPONLIST_C4			6
#define	WEAPONLIST_MAC10		7
#define	WEAPONLIST_AUG			8
#define	WEAPONLIST_SMOKEGRENADE	9
#define	WEAPONLIST_ELITE		10
#define	WEAPONLIST_FIVESEVEN	11
#define	WEAPONLIST_UMP45		12
#define	WEAPONLIST_SG550		13
#define	WEAPONLIST_GALIL		14
#define	WEAPONLIST_FAMAS		15
#define	WEAPONLIST_USP			16
#define	WEAPONLIST_GLOCK18		17
#define	WEAPONLIST_AWP			18
#define	WEAPONLIST_MP5			19
#define	WEAPONLIST_M249			20
#define	WEAPONLIST_M3			21
#define	WEAPONLIST_M4A1			22
#define	WEAPONLIST_TMP			23
#define	WEAPONLIST_G3SG1		24
#define	WEAPONLIST_FLASHBANG	25
#define	WEAPONLIST_DEAGLE		26
#define	WEAPONLIST_SG552		27
#define	WEAPONLIST_AK47			28
#define	WEAPONLIST_KNIFE		29
#define	WEAPONLIST_P90			30

#define USP_SILENCER (1 << 0)
#define GLOCK18_BURST (1 << 1)
#define M4A1_SILENCER (1 << 2)

/*
pfnUserMsgHook pTeamInfo = NULL;
pfnUserMsgHook pDeathMsg = NULL;
pfnUserMsgHook pCurWeapon = NULL;
pfnUserMsgHook pSetFOV = NULL;
*/
//cl_enginefunc_t *g_pEngine = NULL;

int HookUserMsg( char *szMsgName, pfnUserMsgHook pfn );


typedef int					( *INITIALIZE_FUNCTION )					( struct cl_enginefuncs_s*, int );
typedef void				( *HUD_INIT_FUNCTION )						( void );
typedef int					( *HUD_VIDINIT_FUNCTION )					( void );
typedef int					( *HUD_REDRAW_FUNCTION )					( float, int );
typedef int					( *HUD_UPDATECLIENTDATA_FUNCTION )			( struct client_data_s*, float );
typedef void				( *HUD_RESET_FUNCTION )						( void );
typedef void				( *HUD_CLIENTMOVE_FUNCTION )				( struct playermove_s *ppmove, qboolean server );
typedef void				( *HUD_CLIENTMOVEINIT_FUNCTION )			( struct playermove_s *ppmove );
typedef char				( *HUD_TEXTURETYPE_FUNCTION )				( char *name );
typedef void				( *HUD_IN_ACTIVATEMOUSE_FUNCTION )			( void );
typedef void				( *HUD_IN_DEACTIVATEMOUSE_FUNCTION )		( void );
typedef void				( *HUD_IN_MOUSEEVENT_FUNCTION )				( int mstate );
typedef void				( *HUD_IN_CLEARSTATES_FUNCTION )			( void );
typedef void				( *HUD_IN_ACCUMULATE_FUNCTION )				( void );
typedef void				( *HUD_CL_CREATEMOVE_FUNCTION )				( float frametime, struct usercmd_s *cmd, int active );
typedef int					( *HUD_CL_ISTHIRDPERSON_FUNCTION )			( void );
typedef void				( *HUD_CL_GETCAMERAOFFSETS_FUNCTION)		( float *offs );
typedef struct kbutton_s *	( *HUD_KB_FIND_FUNCTION )					( const char *name );
typedef void				( *HUD_CAMTHINK_FUNCTION )					( void );
typedef void				( *HUD_V_CALCREFDEF_FUNCTION )				( struct ref_params_s *pparams );
typedef int					( *HUD_ADDENTITY_FUNCTION )					( int type, struct cl_entity_s *ent, const char *modelname );
typedef void				( *HUD_CREATEENTITIES_FUNCTION )			( void );
typedef void				( *HUD_DRAWNORMALTRIS_FUNCTION )			( void );
typedef void				( *HUD_DRAWTRANSTRIS_FUNCTION )				( void );
typedef void				( *HUD_STUDIOEVENT_FUNCTION )				( const struct mstudioevent_s *event, const struct cl_entity_s *entity );
typedef void				( *HUD_POSTRUNCMD_FUNCTION )				( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed );
typedef void				( *HUD_SHUTDOWN_FUNCTION )					( void );
typedef void				( *HUD_TXFERLOCALOVERRIDES_FUNCTION )		( struct entity_state_s *state, const struct clientdata_s *client );
typedef void				( *HUD_PROCESSPLAYERSTATE_FUNCTION )		( struct entity_state_s *dst, const struct entity_state_s *src );	
typedef void				( *HUD_TXFERPREDICTIONDATA_FUNCTION )		( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd );
typedef void				( *HUD_DEMOREAD_FUNCTION )					( int size, unsigned char *buffer );
typedef int					( *HUD_CONNECTIONLESS_FUNCTION )			( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
typedef int					( *HUD_GETHULLBOUNDS_FUNCTION )				( int hullnumber, float *mins, float *maxs );
typedef void				( *HUD_FRAME_FUNCTION )						( double );
typedef int					( *HUD_KEY_EVENT_FUNCTION )					( int eventcode, int keynum, const char *pszCurrentBinding );
typedef void				( *HUD_TEMPENTUPDATE_FUNCTION )				( double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, 	int ( *Callback_AddVisibleEntity )( struct cl_entity_s *pEntity ),	void ( *Callback_TempEntPlaySound )( struct tempent_s *pTemp, float damp ) );
typedef struct cl_entity_s *( *HUD_GETUSERENTITY_FUNCTION )				( int index );
typedef void				( *HUD_VOICESTATUS_FUNCTION )				( int entindex, qboolean bTalking );
typedef void				( *HUD_DIRECTORMESSAGE_FUNCTION )			( int iSize, void *pbuf );
typedef int					( *HUD_STUDIO_INTERFACE_FUNCTION )			( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio );
typedef void				( *HUD_CHATINPUTPOSITION_FUNCTION )			( int *x, int *y );
typedef int					( *HUD_GETPLAYERTEAM_FUNCTION )				( int iplayer );
typedef void				( *CLIENTFACTORY )							( );
//==================================================================================
// Client API  structure. Holds all functions exported by client.
//==================================================================================
typedef struct _CLIENT_
{
	INITIALIZE_FUNCTION								Initialize;
	HUD_INIT_FUNCTION								HUD_Init;
	HUD_VIDINIT_FUNCTION							HUD_VidInit;
	HUD_REDRAW_FUNCTION								HUD_Redraw;
	HUD_UPDATECLIENTDATA_FUNCTION					HUD_UpdateClientData;
	HUD_RESET_FUNCTION								HUD_Reset;
	HUD_CLIENTMOVE_FUNCTION							HUD_PlayerMove;
	HUD_CLIENTMOVEINIT_FUNCTION						HUD_PlayerMoveInit;
	HUD_TEXTURETYPE_FUNCTION						HUD_PlayerMoveTexture;
	HUD_IN_ACTIVATEMOUSE_FUNCTION					IN_ActivateMouse;
	HUD_IN_DEACTIVATEMOUSE_FUNCTION					IN_DeactivateMouse;
	HUD_IN_MOUSEEVENT_FUNCTION						IN_MouseEvent;
	HUD_IN_CLEARSTATES_FUNCTION						IN_ClearStates;
	HUD_IN_ACCUMULATE_FUNCTION						IN_Accumulate;
	HUD_CL_CREATEMOVE_FUNCTION						CL_CreateMove;
	HUD_CL_ISTHIRDPERSON_FUNCTION					CL_IsThirdPerson;
	HUD_CL_GETCAMERAOFFSETS_FUNCTION				CL_CameraOffset;
	HUD_KB_FIND_FUNCTION							KB_Find;
	HUD_CAMTHINK_FUNCTION							CAM_Think;
	HUD_V_CALCREFDEF_FUNCTION						V_CalcRefdef;
	HUD_ADDENTITY_FUNCTION							HUD_AddEntity;
	HUD_CREATEENTITIES_FUNCTION						HUD_CreateEntities;
	HUD_DRAWNORMALTRIS_FUNCTION						HUD_DrawNormalTriangles;
	HUD_DRAWTRANSTRIS_FUNCTION						HUD_DrawTransparentTriangles;
	HUD_STUDIOEVENT_FUNCTION						HUD_StudioEvent;
	HUD_POSTRUNCMD_FUNCTION							HUD_PostRunCmd;
	HUD_SHUTDOWN_FUNCTION							HUD_Shutdown;
	HUD_TXFERLOCALOVERRIDES_FUNCTION				HUD_TxferLocalOverrides;
	HUD_PROCESSPLAYERSTATE_FUNCTION					HUD_ProcessPlayerState;
	HUD_TXFERPREDICTIONDATA_FUNCTION				HUD_TxferPredictionData;
	HUD_DEMOREAD_FUNCTION							Demo_ReadBuffer;
	HUD_CONNECTIONLESS_FUNCTION						HUD_ConnectionlessPacket;
	HUD_GETHULLBOUNDS_FUNCTION						HUD_GetHullBounds;
	HUD_FRAME_FUNCTION								HUD_Frame;
	HUD_KEY_EVENT_FUNCTION							HUD_Key_Event;
	HUD_TEMPENTUPDATE_FUNCTION						HUD_TempEntUpdate;
	HUD_GETUSERENTITY_FUNCTION						HUD_GetUserEntity;
	HUD_VOICESTATUS_FUNCTION						HUD_VoiceStatus;
	HUD_DIRECTORMESSAGE_FUNCTION					HUD_DirectorMessage;
	HUD_STUDIO_INTERFACE_FUNCTION					HUD_GetStudioModelInterface;
	HUD_CHATINPUTPOSITION_FUNCTION					HUD_ChatInputPosition;
	HUD_GETPLAYERTEAM_FUNCTION						HUD_GetPlayerTeam;
	CLIENTFACTORY									ClientFactory;
} CLIENT, *PCLIENT;


// client HANDLERS
void PostV_CalcRefdef ( struct ref_params_s *pparams );
void PreV_CalcRefdef ( struct ref_params_s *pparams );
void V_CalcRefdef ( struct ref_params_s *pparams );
void CL_CreateMove ( float frametime, struct usercmd_s *cmd, int active);
void HUD_Frame( double time );
void HUD_Redraw ( float x, int y );
void HUD_PostRunCmd ( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed );
void HUD_PlayerMove ( struct playermove_s *ppmove, qboolean server );
void HUD_Init ( void );
int HUD_AddEntity ( int type, struct cl_entity_s *ent, const char *modelname );
int HUD_Key_Event ( int eventcode, int keynum, const char *pszCurrentBinding );
void HUD_UpdateClientData(client_data_t *cdata, float flTime);
int Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion );
int HUD_GetStudioModelInterface ( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio );

// engine HANDLERS
HCSPRITE SPR_Load ( const char *szPicName );
void SPR_Set ( HCSPRITE hPic, int r, int g, int b );
void SPR_Draw ( int frame, int x, int y, const struct rect_s *prc );
void SPR_DrawHoles ( int frame, int x, int y, const struct rect_s *prc );
void SPR_DrawAdditive ( int frame, int x, int y, const struct rect_s *prc );
int DrawCharacter ( int x, int y, int number, int r, int g, int b );
int DrawConsoleString ( int x, int y, char *string );
void FillRGBA ( int x, int y, int width, int height, int r, int g, int b, int a );

// user message HANDLERS
int TeamInfo (const char *pszName, int iSize, void *pbuf);
int CurWeapon(const char *pszName, int iSize, void *pbuf );
int ScoreAttrib(const char *pszName, int iSize, void *pbuf );
int SetFOV(const char *pszName, int iSize, void *pbuf);
int Health(const char *pszName, int iSize, void *pbuf);
int Battery(const char *pszName, int iSize, void *pbuf);
int ScoreInfo(const char *pszName, int iSize, void *pbuf);
int DeathMsg(const char *pszName, int iSize, void *pbuf);
int SayText(const char *pszName, int iSize, void *pbuf);
int TextMsg(const char *pszName, int iSize, void *pbuf);
int ResetHUD(const char *pszName, int iSize, void *pbuf);
int Damage(const char *pszName, int iSize, void *pbuf );
int AmmoX(const char *pszName, int iSize, void *pbuf );
int WeaponListMsg(const char *pszName, int iSize, void *pbuf );
int Money(const char *pszName, int iSize, void *pbuf);



// activation functions
BOOL ActivateClient();
BOOL ActivateEngine();

extern bool oglSubtractive;

#endif