#include <Windows.h>
#include "gamehooking.h"
#include "players.h"

#define BOUND_VALUE(var,min,max) if((var)>(max)){(var)=(max);};if((var)<(min)){(var)=(min);}

int			displayCenterX = 640;
int			displayCenterY = 480;
float		fCurrentFOV = 90;
SCREENINFO	screeninfo;

float mainViewAngles[3];

sMe me;
VecPlayers vPlayers;

int Cstrike_SequenceInfo[] = 
{
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0, // 0..9   
	0,	1,	2,	0,	1,	2,	0,	1,	2,	0, // 10..19 
	1,	2,	0,	1,	1,	2,	0,	1,	1,	2, // 20..29 
	0,	1,	2,	0,	1,	2,	0,	1,	2,	0, // 30..39 
	1,	2,	0,	1,	2,	0,	1,	2,	0,	1, // 40..49 
	2,	0,	1,	2,	0,	0,	0,	8,	0,	8, // 50..59 
	0, 16,	0, 16,	0,	0,	1,	1,	2,	0, // 60..69 
	1,	1,	2,	0,	1,	0,	1,	0,	1,	2, // 70..79 
	0,	1,	2, 	32, 40, 32, 40, 32, 32, 32, // 80..89
   	33, 64, 33, 34, 64, 65, 34, 32, 32, 4, // 90..99
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4, // 100..109
	4                                      	// 110
};

//////////////////////////////////////////////////////////////////////////
void InitVisuals(void)
{
	static bool bOnce = false;
	if(bOnce) return;
	screeninfo.iSize = sizeof(SCREENINFO);
	gEngfuncs.pfnGetScreenInfo(&screeninfo);
	displayCenterX = screeninfo.iWidth/2;
	displayCenterY = screeninfo.iHeight/2;
	bOnce = true;
}

//////////////////////////////////////////////////////////////////////////
bool CalcScreen(const float *origin, float *vecScreen)
{
	int result = gEngfuncs.pTriAPI->WorldToScreen( const_cast<float*>(origin),vecScreen);

	if(vecScreen[0] < 1 && vecScreen[1] < 1 && vecScreen[0] > -1 && vecScreen[1] > -1 && !result)
	{
		vecScreen[0] =  vecScreen[0] * displayCenterX + displayCenterX;
		vecScreen[1] = -vecScreen[1] * displayCenterY + displayCenterY;
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
void gDrawBoxAtScreenXY( int x, int y, int r, int g, int b, int alpha, int radius=1)
{
	int radius2 = radius<<1;
	gEngfuncs.pfnFillRGBA(x-radius+2, y-radius, radius2-2,2,r,g,b,alpha);
	gEngfuncs.pfnFillRGBA(x-radius, y-radius, 2,radius2,r,g,b,alpha);
	gEngfuncs.pfnFillRGBA(x-radius, y+radius, radius2,2,r,g,b,alpha);
	gEngfuncs.pfnFillRGBA(x+radius, y-radius, 2,radius2+2,r,g,b,alpha);
}



void drawPlayerEsp(int ax)
{
	if(strstr(vPlayers[ax].getName(), "\\noname\\")) return;

	
	// rest of code is only for players on screen:
	float vecScreen[2];
	if( !CalcScreen(vPlayers[ax].origin(),vecScreen) ) { return; }	

	// distance and boxradius also needed for esp offset
	float distance = vPlayers[ax].distance/22.0f;
	extern float fCurrentFOV;
	int   boxradius = (300.0*90.0) / (distance*fCurrentFOV);	 
	BOUND_VALUE(boxradius,1,200);

	gDrawBoxAtScreenXY(vecScreen[0],vecScreen[1],0,255,0,150,boxradius);
	
}


//////////////////////////////////////////////////////////////////////////
static inline void strcpy_x2(char* dest, const char* pos)
{
	do{ 
		if(*pos=='.'||*pos=='-'||*pos=='_') {*dest=0;break; } // cut off ".wav"
		*dest=*pos;
		++dest;
		++pos; 
	}
	while(*pos);
}

//////////////////////////////////////////////////////////////////////////
static inline float GetDistanceFrom(const float* pos);
void playerSound(int index, const float*const origin, const char*const sample)
{
	UpdateMe();
	if(index == me.ent->index) return;

	// fill infos:
	PlayerInfo& player = vPlayers[index];

	// calc player data 
	player.updateEntInfo();
	player.updateSoundRadar();
	player.SetOrigin(origin);
	
	player.distance  = GetDistanceFrom (player.origin());
	if(player.distance<1)
		player.distance=1; // avoid division by zero
	
	player.visible   = false;

	// set weapon
	if( sample && sample[0]=='w' && sample[1]=='e' && sample[7]=='/' ) // ex: weapons/ak-1.wav
	{
		const char* pos = sample+8;
		if(strlen(pos)<30)
		{
			char tmp[32];
			strcpy_x2(tmp,pos);

			if(!strcmp(tmp,"zoom")){ strcpy(tmp,"awp"); } // zooming means usually awp

			if(strcmp(tmp,"knife")) // dont update for knife
			{
				player.setWeapon(tmp);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void UpdateMe(void)
{
	me.ent    = gEngfuncs.GetLocalPlayer();

	static cl_entity_s dummy;
	memset((char*)&dummy,0,sizeof(dummy));
	if(!me.ent){ me.ent = &dummy; }
}

//////////////////////////////////////////////////////////////////////////
bool isValidEnt(cl_entity_s *ent) 
{

	if(ent && (ent != gEngfuncs.GetLocalPlayer())  && ent->player && !ent->curstate.spectator &&
		ent->curstate.solid && !(ent->curstate.messagenum < gEngfuncs.GetLocalPlayer()->curstate.messagenum)) 
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
// fix for ogc
bool bIsEntValid(cl_entity_s * ent,int index)
{

	if (vPlayers[index].updateType() == 2 || vPlayers[index].timeSinceLastUpdate() < 0.3)
		return true;
	UpdateMe(); // Avoid Crash
	cl_entity_s* localEnt = gEngfuncs.GetEntityByIndex(me.ent->index);

	if(ent && !(ent->curstate.effects & EF_NODRAW) && ent->player && !ent->curstate.spectator 
		&& ent->curstate.solid && !(ent->curstate.messagenum < localEnt->curstate.messagenum))
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
void playerCalcExtraData(int ax, cl_entity_s* ent)
{
	PlayerInfo& r = vPlayers[ax];

	// playername and model
	r.updateEntInfo();

	// distance
	r.distance  = GetDistanceFrom (r.origin());
	
	if(r.distance<1)
		r.distance=1; // avoid division by zero
	
	vPlayers[ax].bGotHead = false;
}

char* gGetWeaponName( int weaponmodel ) 
{ 
    static char weapon[50]; 
    weapon[0]=0; 
    model_s* mdl = pStudio->GetModelByIndex( weaponmodel ); 
    if( !mdl ){ return weapon; } 
    char* name = mdl->name;  if( !name )          { return weapon; } 
     
    int len = strlen(name);  if( len>48 || len<10){ return weapon; } 
    if (len > 25)//to catch shield string 
    { 
        strcpy(weapon,name+23); len -= 23; 
    } 
    else//no shield 
    { 
        strcpy(weapon,name+9); len -=9; 
    } 
    if(len>4)weapon[len-4]=(char)0; 
        return weapon; 
} 

//////////////////////////////////////////////////////////////////////////
static inline float GetDistanceFrom(const float* pos)
{	
	register double a = pos[0] - me.pmEyePos[0];
	register double b = pos[1] - me.pmEyePos[1];
	register double c = pos[2] - me.pmEyePos[2];
	return sqrt(a*a + b*b + c*c);
}


//==================================================================================
// AtMapChange
//==================================================================================
void AtMapChange()
{
	me.iKills = 0;
	me.iHs = 0;
	me.iDeaths = 0;
}

//==================================================================================
// AtRoundStart - Called every start of a round
//==================================================================================
void AtRoundStart()
{
	static char currentMap[100];
	if( strcmp(currentMap,gEngfuncs.pfnGetLevelName()) )
	{
		strcpy(currentMap,gEngfuncs.pfnGetLevelName());
		AtMapChange();
	}

	for(int i=0;i<MAX_VPLAYERS;i++) 
	{ 
		vPlayers[i].updateClear();
		vPlayers[i].bGotHead = false;
		vPlayers[i].setAlive();
	}

}

bool bIsEntValid(cl_entity_s * ent)
{
	cl_entity_s* localEnt = gEngfuncs.GetLocalPlayer();

	if(ent && ent != localEnt && !(ent->curstate.effects & EF_NODRAW) && ent->player && !ent->curstate.spectator 
		&& ent->curstate.solid && !(ent->curstate.messagenum < localEnt->curstate.messagenum))
		return true;

	return false;
}

bool ValidTarget(cl_entity_s* ent)
{
	int nPlayers = vPlayers.size();
	if(!bIsEntValid(ent)) { return false; }

	int entindex = ent->index;

	for(int px = 0; px < nPlayers; px++)
	{
		PlayerInfo& pi = vPlayers[px];

		if( (pi.getEnt() == ent) && (pi.team != me.team) )
		{
			return true;
		}
	}

	return false;
}

/*
void sMe::DoBunnyHop(struct usercmd_s *usercmd)
{
	if(!(me.pmFlags & FL_ONGROUND)) 
	{
		//gEngfuncs.pfnClientCmd("-jump");
		usercmd->buttons &= ~IN_JUMP;
	}
	else if(me.pmFlags & FL_ONGROUND)
	{
		//gEngfuncs.pfnClientCmd("+jump");
		usercmd->buttons &= IN_JUMP;
	}
}*/

void sMe::DoBunnyHop(struct usercmd_s *usercmd)
{
    if((usercmd->buttons & IN_JUMP) && !(me.pmFlags & FL_ONGROUND)) 
    {
        usercmd->buttons &= ~IN_JUMP;
    }
}

int getSeqInfo(int ax)
{
	return Cstrike_SequenceInfo[vPlayers[ax].getEnt()->curstate.sequence];
}