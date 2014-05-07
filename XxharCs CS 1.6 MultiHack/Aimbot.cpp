//==============================================================================
// Aimbot.cpp
//==============================================================================

#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4786)
#pragma warning(disable:4305)
#pragma warning(disable:4800)
#pragma warning(disable:4244)
#pragma warning(disable:4101)
#pragma warning(disable:4715)
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#undef NDEBUG
#include <assert.h>
#include <memory.h>
#include <map>
#include <vector>
#include <fstream>
#include "gamehooking.h"
#include "aimbot.h"
#include "cvars.h"
#include "players.h"
#include "weaponlist.h"

using namespace std;

typedef float TransformMatrix[MAXSTUDIOBONES][3][4];
#define M_PI		3.14159265358979323846

CAimbot Aimbot;
float mainViewOrigin[3];

//===================================================================================

float predahead = 0.20; // Some default Values ;D
int predback = 0;
static void PredictTarget(int index,float *pred)
{
//	if (cvar.pred)
//	{
	/*		cl_entity_s* ent = gEngfuncs.GetEntityByIndex(index);
			int  historyIndex = (ent->current_position+HISTORY_MAX-predback)%HISTORY_MAX;
			
			vec3_t vFromOrigin , vToOrigin , vDeltaOrigin,vPredictedOrigin;

			vFromOrigin = ent->ph[historyIndex].origin;
			vToOrigin   = ent->ph[ent->current_position].origin;
			vDeltaOrigin = vToOrigin - vFromOrigin;

			vDeltaOrigin[0] *= predahead;
			vDeltaOrigin[1] *= predahead;
			vDeltaOrigin[2] *= predahead;

			vPredictedOrigin = ent->origin + vDeltaOrigin;
			VectorCopy(vPredictedOrigin,pred); */
	cl_entity_s* ent = gEngfuncs.GetEntityByIndex(index);
	VectorCopy(ent->origin, pred);
/*	}
	else 
	{
			VectorCopy(vPlayers[index].getEnt()->origin,pred);
	}*/
}

//==============================================================================
float vpreahead = 1.2;
bool bSoonvisible(int iIndex)
{
	float to[3], lastpred = predahead;
	predahead = vpreahead;// apply some leeb value here :)
	PredictTarget(iIndex,(float*)to);
	predahead = lastpred;
	pmtrace_t tr;
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( me.pmEyePos, to, PM_WORLD_ONLY, me.ent->index, &tr );
//	return ( tr.fraction == 1.0 ); 
	if (tr.fraction == 0.75 )
		return true;
	return false;
}

//==============================================================================

void VectorAngles( const float *forward, float *angles )
{
	float tmp, yaw, pitch;
	
	if( forward[1] == 0 && forward[0] == 0 )
	{
		yaw = 0;

		if( forward[2] > 0 )
			pitch = 90.0f;
		else
			pitch = 270.0f;
	}
	else
	{
		yaw = ( float )( ( atan2( forward[1], forward[0] ) * 180 / M_PI ) );

		if( yaw < 0 )
			yaw += 360.0f;

		tmp = sqrt( forward[0] * forward[0] + forward[1] * forward[1] );
		
		pitch = ( float )( ( atan2( forward[2], tmp ) * 180 / M_PI ) );
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

//==============================================================================
int CanPenetrate( float *start, float *end, int power ) 
{ 
   pmtrace_t pmtrace; 
   pmtrace_t * tr = (pmtrace_t*) &pmtrace;

   float view[3]; 
   float dir[3]; 

   view[0] = end[0] - start[0]; 
   view[1] = end[1] - start[1]; 
   view[2] = end[2] - start[2]; 

   float length = VectorLength(view); 

   dir[0] = view[0] / length; 
   dir[1] = view[1] / length; 
   dir[2] = view[2] / length; 

   float position[3]; 
   position[0] = start[0]; 
   position[1] = start[1]; 
   position[2] = start[2]; 
   tr->startsolid = true; 

   while( power ) 
   { 
      if( !tr->startsolid ) 
         power--; 
      tr = gEngfuncs.PM_TraceLine( position, end, PM_TRACELINE_PHYSENTSONLY, 2, -1); 
       
      if( tr->fraction==1.0f ) 
         return 1; 
      if( tr->allsolid ) 
         return 0; 

      position[0] = tr->endpos[0] + dir[0] * 8.0f; 
      position[1] = tr->endpos[1] + dir[1] * 8.0f; 
      position[2] = tr->endpos[2] + dir[2] * 8.0f; 
   } 
   return 0; 
} 
//==============================================================================
void VectorTransform (float *in1, float in2[3][4], float *out)
{
    out[0] = DotProduct(in1, in2[0]) + in2[0][3];
    out[1] = DotProduct(in1, in2[1]) + in2[1][3];
    out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

//==============================================================================
#include <gl\gl.h>

void tintArea(int x,int y,int w,int h,int r,int g,int b,int a)
{
	oglSubtractive = true;
	gEngfuncs.pfnFillRGBA(x,y,w,h,r,g,b,a);
	oglSubtractive = false;
}

void CAimbot::DrawAimSpot(void)
{
	//if(!cvar.avdraw) return;
	for(int i = 0; i < 32; i++)
	{
		if(!vPlayers[i].bDrawn || !vPlayers[i].isUpdatedAddEnt()) continue;
		float fVecScreen[2];
		vec3_t vecEnd, up, right, forward, EntViewOrg, playerAngles;
		if (!vPlayers[i].fixHbAim)		 {VectorCopy(vPlayers[i].vHitbox,EntViewOrg);}	
		else							 {VectorCopy(vPlayers[i].origin(),EntViewOrg);}
		// calculate angle vectors
		playerAngles[0]=0;
		playerAngles[1]=vPlayers[iTarget].getEnt()->angles[1];
		playerAngles[2]=0;
		gEngfuncs.pfnAngleVectors (playerAngles, forward, right, up);
		forward[2] = -forward[2];
		EntViewOrg = EntViewOrg + forward * 0.5;
		EntViewOrg = EntViewOrg + up * 2.5;
		EntViewOrg = EntViewOrg + right * 0.0;
		if(!CalcScreen(EntViewOrg, fVecScreen)) continue;
		tintArea(fVecScreen[0], fVecScreen[1], 2, 2, 255, 255, 255, 255);
	}
}

void CAimbot::CalculateHitbox( cl_entity_s *pEnt )
{
	/*if( !cvar.aimingmethod )
		return;*/
	/*if(cvar.aimingmethod == 1)
	{*/
		if( !vPlayers[pEnt->index].bGotHead )
		{
			int iIndex = pEnt->index;

			model_s		    *pModel			= pStudio->SetupPlayerModel( iIndex );
			studiohdr_t     *pStudioHeader	= ( studiohdr_t* )pStudio->Mod_Extradata( pModel );
			mstudiobbox_t   *pStudioBox;
			TransformMatrix *pBoneTransform = ( TransformMatrix* )pStudio->StudioGetBoneTransform( );	

			vec3_t vMin, vMax;
			pStudioBox = ( mstudiobbox_t* )( ( byte* )pStudioHeader + pStudioHeader->hitboxindex );

			//Head 11 bone 7 | Low Head 9 bone 5 | Chest 8 bone 4 | Stomach 7 bone 3
			int i = 11;
			
			VectorTransform(pStudioBox[i].bbmin, (*pBoneTransform)[pStudioBox[i].bone], vMin);
			VectorTransform(pStudioBox[i].bbmax, (*pBoneTransform)[pStudioBox[i].bone], vMax);
			vPlayers[iIndex].vHitbox	= ( vMin + vMax ) * 0.5f;
			vPlayers[iIndex].bGotHead	= true;		
		}
	/*}*/
	/*if(cvar.aimingmethod == 2)
	{
		int ax = pEnt->index;
		if(!vPlayers[ax].bGotHead)
		{
			vec3_t pos;
	//		studiohdr_t* pStudioHeader = (studiohdr_t*)oEngStudio.Mod_Extradata( pEnt->model );
			TransformMatrix*  pbonetransform = (TransformMatrix*)pStudio->StudioGetBoneTransform();
			//Head 11 bone 7 | Low Head 9 bone 5 | Chest 8 bone 4 | Stomach 7 bone 3
			int i = 7;
			if (cvar.aimspot == 1) i = 7;
			else if (cvar.aimspot == 2) i = 5;
			else if (cvar.aimspot == 3) i = 4;
			pos[ 0 ] = (*pbonetransform)[ i ][ 0 ][ 3 ];
			pos[ 1 ] = (*pbonetransform)[ i ][ 1 ][ 3 ];
			pos[ 2 ] = (*pbonetransform)[ i ][ 2 ][ 3 ];
			VectorCopy(pos, vPlayers[ax].vHitbox);
			vPlayers[ax].bGotHead = true;
		}
	}*/
}

//==============================================================================

void CAimbot::CalculateAimingView( void )
{
	float view[3];
	vec3_t vecEnd, up, right, forward, EntViewOrg, playerAngles;

	if (!vPlayers[iTarget].fixHbAim) {VectorCopy(vPlayers[iTarget].vHitbox,EntViewOrg);}	
	else							 {VectorCopy(vPlayers[iTarget].origin(),EntViewOrg);}	

	// calculate angle vectors
	playerAngles[0]=0;
	playerAngles[1]=vPlayers[iTarget].getEnt()->angles[1];
	playerAngles[2]=0;
	gEngfuncs.pfnAngleVectors (playerAngles, forward, right, up);
	forward[2] = -forward[2];
		
	EntViewOrg = EntViewOrg + forward * 0.5;
	EntViewOrg = EntViewOrg + up * 2.5;
	EntViewOrg = EntViewOrg + right * 0.0;

	view[0] = EntViewOrg[0] - me.pmEyePos[0];
	view[1] = EntViewOrg[1] - me.pmEyePos[1];
	view[2] = EntViewOrg[2] - me.pmEyePos[2];

	VectorAngles(view,aim_viewangles);
	aim_viewangles[0] *= -1;

	if (aim_viewangles[0]>180) aim_viewangles[0]-=360;
	if (aim_viewangles[1]>180) aim_viewangles[1]-=360;
}
//==============================================================================
int CorrectGunX() 
{ 
   int currentWeaponID = GetCurWeaponID();
   if (currentWeaponID == WEAPONLIST_SG550 || currentWeaponID == WEAPONLIST_G3SG1 || currentWeaponID == WEAPONLIST_SCOUT || currentWeaponID == WEAPONLIST_AWP)
   { return 3; }

   if (currentWeaponID == WEAPONLIST_AUG || currentWeaponID == WEAPONLIST_UNKNOWN1 || currentWeaponID == WEAPONLIST_UNKNOWN2 ||  currentWeaponID == WEAPONLIST_DEAGLE || currentWeaponID == WEAPONLIST_SG552 || currentWeaponID == WEAPONLIST_AK47) 
   { return 2; }
    
   { return 1; }
}
//====================================================================================
bool CAimbot::pathFree(float* xfrom,float* xto)
{ 
   int pathtest; 
   pmtrace_t tr; 
   gEngfuncs.pEventAPI->EV_SetTraceHull( 2 ); 
   gEngfuncs.pEventAPI->EV_PlayerTrace( xfrom, xto, PM_GLASS_IGNORE, me.ent->index, &tr ); 
   pathtest = (tr.fraction == 1.0); 
   if (!pathtest && CorrectGunX()) 
   { 
      pathtest = CanPenetrate(xfrom, xto, CorrectGunX()); 
   } 
   return pathtest; 
} 
//====================================================================================

bool CAimbot::TargetRegion(int ax)
{
	vec3_t vecEnd, up, right, forward, EntViewOrg,playerAngles;
	cl_entity_s* ent = vPlayers[ax].getEnt();
	
    // calculate angle vectors
	playerAngles[0]=0;
	playerAngles[1]=ent->angles[1];
	playerAngles[2]=0;
    gEngfuncs.pfnAngleVectors (playerAngles, forward, right, up);
	forward[2] = -forward[2];

	if (vPlayers[ax].bGotHead)	{VectorCopy(vPlayers[ax].vHitbox,EntViewOrg);vPlayers[ax].fixHbAim=false;}	
	else						{VectorCopy(vPlayers[ax].origin(),EntViewOrg);vPlayers[ax].fixHbAim=true;}		

	EntViewOrg = EntViewOrg + forward * 0.5;
	EntViewOrg = EntViewOrg + up * 2.5;
    EntViewOrg = EntViewOrg + right * 0.0;
		
	if(pathFree(me.pmEyePos,EntViewOrg) || pathFree(me.pmEyePos, ent->origin)) return true;

	return false;
}

//==============================================================================
/*
bool isValidEnt(cl_entity_s *ent) 
{
	if(ent && (ent != gEngfuncs.GetLocalPlayer())  && ent->player && !ent->curstate.spectator && ent->curstate.solid && !(ent->curstate.messagenum < gEngfuncs.GetLocalPlayer()->curstate.messagenum)) 
		return true;
	else 
		return false;
}*/
//==============================================================================
void CAimbot::FindTarget( void )
{
	if (!me.alive) return;	

	SetTarget(-1);

	for (int ax=0;ax<MAX_VPLAYERS;ax++)
	{
		if  ( vPlayers[ax].isUpdatedAddEnt() && vPlayers[ax].canAim && vPlayers[ax].isAlive()) // No Dead People
		{
			if( !HasTarget() )											{ SetTarget(ax); continue; }
			if( vPlayers[ax].fovangle <   vPlayers[iTarget].fovangle )	{ SetTarget(ax);	  	   }		
		} 
	}
}

//==============================================================================

float CAimbot::calcFovAngle(const float* origin_viewer, const float* angle_viewer, const float* origin_target)
{
	double vec[3], view[3];
	double dot;	
	
	view[0] = origin_target[0] - origin_viewer[0];
	view[1] = origin_target[1] - origin_viewer[1];
	view[2] = origin_target[2] - origin_viewer[2];

	dot = sqrt(view[0] * view[0] + view[1] * view[1] + view[2] * view[2]);
	dot = 1/dot;

	vec[0] = view[0] * dot;
	vec[1] = view[1] * dot;
	vec[2] = view[2] * dot;
	
	view[0] =  sin((angle_viewer[1] + 90) * (M_PI / 180));
	view[1] = -cos((angle_viewer[1] + 90) * (M_PI / 180));
	view[2] = -sin( angle_viewer[0] * (M_PI / 180));	
	
	dot = view[0] * vec[0] + view[1] * vec[1] + view[2] * vec[2];
	
	// dot to angle:
	return (float)((1.0-dot)*180.0); 
}

//==============================================================================

bool CAimbot::CheckTeam(int ax)
{
	if (me.team != vPlayers[ax].team) return true;
	return false;
}

bool bAim = false;
char* gGetWeaponName( int weaponmodel );
int DoHLHAiming(int eventcode)
{
	char *szWeapon;
	UpdateMe();
	szWeapon = gGetWeaponName(me.ent->curstate.weaponmodel);
	if(strstr(szWeapon, "nade") || strstr(szWeapon, "c4") || strstr(szWeapon, "flashbang")) return 1;
	if (eventcode == 1)
	{
		bAim = true;
		gEngfuncs.pfnClientCmd("+attack");	
		return 0;
	}
	else
	{
		bAim = false;
		gEngfuncs.pfnClientCmd("-attack");
		return 1;
	}
}
//==============================================================================
/*extern float gSpeed;
int DoSpeed(int eventcode)
{
	if (eventcode == 1)
	{
		if (cvar.knivespeed && IsCurWeaponKnife())	gSpeed = 20.0f;
		else										gSpeed = cvar.speed;		
	}
	else
		gSpeed = 0.0;
	return 0;
}*/

//==============================================================================
int getSeqInfo(int ax);
bool CAimbot::IsShielded(int ax)
{
	int seqinfo = getSeqInfo(ax);
	if (seqinfo & SEQUENCE_RELOAD)
		return false;
	if (seqinfo & SEQUENCE_SHIELD)
		return true;
	return false;
}

//==============================================================================

void CAimbot::calcFovangleAndVisibility(int ax)
{ 
	PlayerInfo& r = vPlayers[ax];
	r.fovangle    = calcFovAngle(me.pmEyePos, me.viewAngles, r.origin() );
	
	if(r.updateType() == 0 || r.updateType() == 2 || !r.isAlive()) { r.visible = false; return; }

	float fov = 90; //1=10 , 2=30 , 3=90 , 4=360
	

	r.visible = TargetRegion(ax);
	
	if(0) {}
	else if (!CheckTeam(ax))					{ r.canAim = 0; }
	else if (IsShielded(ax))					{ r.canAim = 0; }
	else if (r.fovangle>fov)					{ r.canAim = 0; }	
	else if (bSoonvisible(ax))					{ r.canAim = 1; }
	/*else if (cvar.autowall)
	{
		int damage = GetDamageVec(ax, true);
		if (damage)
		{
			r.canAim = 2;
		}
		else
		{
			damage = GetDamageVec(ax, false);
			if (damage)
				r.canAim = 1;
			else
				r.canAim = 0;
		}
	}*/
	else										{ r.canAim = r.visible; }
}
//==============================================================================

int CAimbot::GetDamageVec(int ax, bool onlyvis)
{
	int hitdamage, penetration = WALL_PEN0;
	vec3_t vecEnd, up, right, forward, EntViewOrg, PlayerOrigin, playerAngles, targetspot;

	VectorCopy(vPlayers[ax].vHitbox,PlayerOrigin);

	playerAngles[0]=0;
	playerAngles[1]=vPlayers[ax].getEnt()->angles[1];
	playerAngles[2]=0;

    gEngfuncs.pfnAngleVectors(playerAngles, forward, right, up);

	forward[2] = -forward[2];

	if (!onlyvis)
		penetration = CorrectGunX();

	targetspot[0] = PlayerOrigin[0] + up[0] * 2.5 + forward[0] * 0.5 + right[0] * 0.0;
	targetspot[1] = PlayerOrigin[1] + up[1] * 2.5 + forward[1] * 0.5 + right[1] * 0.0;
	targetspot[2] = PlayerOrigin[2] + up[2] * 2.5 + forward[2] * 0.5 + right[2] * 0.0;

	hitdamage = CanPenetrate(me.pmEyePos, targetspot, penetration);
	if (hitdamage > 0)
		return hitdamage;

	return 0;
}
//==============================================================================
#define SPIN_REVS_PER_SECOND 6.0f   // adjust to taste 
void CAimbot::FixupAngleDifference(usercmd_t *usercmd) 
{ 
    // thanks tetsuo for this copy/paste 
    cl_entity_t *pLocal; 
    Vector viewforward, viewright, viewup, aimforward, aimright, aimup, vTemp; 
    float newforward, newright, newup, newmagnitude, fTime; 
    float forward = g_Originalcmd.forwardmove; 
    float right = g_Originalcmd.sidemove; 
    float up = g_Originalcmd.upmove; 

    pLocal = gEngfuncs.GetLocalPlayer(); 
    if(!pLocal) 
        return; 

// this branch makes sure your horizontal velocity is not affected when fixing up the movement angles -- it isn't specific to spinning and you can use it with the source tetsuo posted in his forum too 
if(pLocal->curstate.movetype == MOVETYPE_WALK) 
    { 
        gEngfuncs.pfnAngleVectors(Vector(0.0f, g_Originalcmd.viewangles.y, 0.0f), viewforward, viewright, viewup); 
    } 
    else 
    { 
        gEngfuncs.pfnAngleVectors(g_Originalcmd.viewangles, viewforward, viewright, viewup); 
    } 

    // SPIN!!! 
    int iHasShiftHeld = GetAsyncKeyState(VK_LSHIFT); 
    if(pLocal->curstate.movetype == MOVETYPE_WALK && !iHasShiftHeld && !(usercmd->buttons & IN_ATTACK) && !(usercmd->buttons & IN_USE)) 
    { 
        fTime = gEngfuncs.GetClientTime(); 
        usercmd->viewangles.y = fmod(fTime * SPIN_REVS_PER_SECOND * 360.0f, 360.0f); 
    } 

// this branch makes sure your horizontal velocity is not affected when fixing up the movement angles -- it isn't specific to spinning and you can use it with the source tetsuo posted in his forum too 
    if(pLocal->curstate.movetype == MOVETYPE_WALK) 
    { 
        gEngfuncs.pfnAngleVectors(Vector(0.0f, usercmd->viewangles.y, 0.0f), aimforward, aimright, aimup); 
    } 
    else 
    { 
        gEngfuncs.pfnAngleVectors(usercmd->viewangles, aimforward, aimright, aimup); 
    } 

        newforward = DotProduct(forward * viewforward.Normalize(), aimforward) + DotProduct(right * viewright.Normalize(), aimforward) + DotProduct(up * viewup.Normalize(), aimforward); 
        newright = DotProduct(forward * viewforward.Normalize(), aimright) + DotProduct(right * viewright.Normalize(), aimright) + DotProduct(up * viewup.Normalize(), aimright); 
        newup = DotProduct(forward * viewforward.Normalize(), aimup) + DotProduct(right * viewright.Normalize(), aimup) + DotProduct(up * viewup.Normalize(), aimup); 

    usercmd->forwardmove = newforward; 
    usercmd->sidemove = newright; 
    usercmd->upmove = newup; 
} 