#ifndef AIMBOT_H
#define AIMBOT_H

#include <windows.h>
#include <vector>
#include "cvars.h"

using namespace std;

struct Aimvec
{
	float h;
	float f;
	float r;
};

class CAimbot
{
protected:
	void init() 
	{ 
		iTarget = -1;
		active = false;
	}

public:
	int iTarget;
	bool active;
	vec3_t aim_viewangles;	

	void DrawAimSpot(void);
	void CalculateHitbox( cl_entity_s *pEnt );
	void CalculateAimingView( void );
	inline void SetTarget( int iIndex ){ iTarget = iIndex; }
	bool HasTarget( void ){ return iTarget != -1; }
	void FindTarget( void );
	bool CheckTeam(int ax);
	bool IsShielded(int ax);
	float calcFovAngle(const float* origin_viewer, const float* angle_viewer, const float* origin_target);
	void calcFovangleAndVisibility(int ax);
	bool pathFree(float* xfrom,float* xto);
	bool TargetRegion(int ax);
	int GetDamageVec(int ax, bool onlyvis);
	void FixupAngleDifference(usercmd_t *usercmd);
	usercmd_t g_Originalcmd;

public:
	CAimbot() { init(); }
};

extern CAimbot Aimbot;
extern float mainViewOrigin[3];
extern bool isValidEnt(cl_entity_s *ent);
extern int DoSpeed(int eventcode);
extern int DoHLHAiming(int eventcode);

#endif