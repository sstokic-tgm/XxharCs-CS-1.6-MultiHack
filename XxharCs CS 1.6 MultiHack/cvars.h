#ifndef __CVARS_H__
#define __CVARS_H__

class CCvars
{
public:
	bool dmAimbot;
	bool aimthrough;
	bool norecoil;
	bool nospread;
	bool wallhack;
	bool smokeRemoval;
	bool flashRemoval;
	bool skyRemoval;
	bool lambert;
	bool transparentWalls;
	bool whiteWalls;
	bool wireframe;
	bool wireframeModels;
	bool hud;
	bool bunnyhop;
};

extern CCvars cvars;

#endif