#pragma warning (disable:4786) // vc++ stl "truncated browser info"   
   
#include <Windows.h>   

#include "gamehooking.h"  
#include "engine/wrect.h"   
#include "engine/cl_dll.h"   
#include "engine/event_api.h"   
#include "engine/pmtrace.h"   
#include "engine/pm_defs.h"   
#include "engine/cl_entity.h"   
#include "eventhook.h" 
#include "players.h"
   
   
   
   
void (*glock1Org)(struct event_args_s *args);   
void (*glock2Org)(struct event_args_s *args);   
void (*shotgun1Org)(struct event_args_s *args);   
void (*shotgun2Org)(struct event_args_s *args);   
void (*mp5Org)(struct event_args_s *args);   
void (*pythonOrg)(struct event_args_s *args);   
void (*gaussOrg)(struct event_args_s *args);   
void (*gaussspinOrg)(struct event_args_s *args);   
void (*trainOrg)(struct event_args_s *args);   
void (*vehicleOrg)(struct event_args_s *args);   
void (*uspOrg)(struct event_args_s *args);   
void (*mp5nOrg)(struct event_args_s *args);   
void (*ak47Org)(struct event_args_s *args);   
void (*augOrg)(struct event_args_s *args);   
void (*deagleOrg)(struct event_args_s *args);   
void (*g3sg1Org)(struct event_args_s *args);   
void (*sg550Org)(struct event_args_s *args);   
void (*glock18Org)(struct event_args_s *args);   
void (*m249Org)(struct event_args_s *args);   
void (*m3Org)(struct event_args_s *args);   
void (*m4a1Org)(struct event_args_s *args);   
void (*mac10Org)(struct event_args_s *args);   
void (*p90Org)(struct event_args_s *args);   
void (*p228Org)(struct event_args_s *args);   
void (*awpOrg)(struct event_args_s *args);   
void (*scoutOrg)(struct event_args_s *args);   
void (*sg552Org)(struct event_args_s *args);   
void (*tmpOrg)(struct event_args_s *args);   
void (*fivesevenOrg)(struct event_args_s *args);   
void (*ump45Org)(struct event_args_s *args);   
void (*xm1014Org)(struct event_args_s *args);   
void (*elite_leftOrg)(struct event_args_s *args);   
void (*elite_rightOrg)(struct event_args_s *args);   
void (*knifeOrg)(struct event_args_s *args);   
void (*decal_resetOrg)(struct event_args_s *args);   
void (*createsmokeOrg)(struct event_args_s *args);   
   
   
void Event_usp(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.prevtime)   
        {   
            me.spread.spreadvar = me.spread.spreadvar - (0.275 * (0.3 - (me.spread.gtime - me.spread.prevtime)));   
   
            if (me.spread.spreadvar > 0.92)   
                me.spread.spreadvar = 0.92f;   
            else if (me.spread.spreadvar < 0.6)   
                me.spread.spreadvar = 0.6f;   
        }   
   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    uspOrg(args);   
}   
   
   
void Event_mp5n(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
   
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * 0.004543389368468878 + 0.45;   
   
        if (me.spread.spreadvar > 0.75)   
            me.spread.spreadvar = 0.75f;   
   
        me.spread.prevtime = me.spread.gtime;   
        me.spread.firing = true;   
    }   
   
    mp5nOrg(args);   
}   
   
   
void Event_ak47(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
   
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * me.spread.recoil / 200 + 0.35;   
   
        if (me.spread.spreadvar > 1.25)   
            me.spread.spreadvar = 1.25f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    ak47Org(args);   
}   
   
   
void Event_aug(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
   
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * me.spread.recoil / 215 + 0.3;   
   
        if (me.spread.spreadvar > 1.0)   
            me.spread.spreadvar = 1.0f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    augOrg(args);   
}   
   
   
void Event_deagle(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.prevtime)   
        {   
            me.spread.spreadvar = me.spread.spreadvar - (0.35 * (0.4 - (me.spread.gtime - me.spread.prevtime)));   
   
            if (me.spread.spreadvar > 0.9)   
                me.spread.spreadvar = 0.9f;   
            else if (me.spread.spreadvar < 0.55)   
                me.spread.spreadvar = 0.55f;   
        }   
   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    deagleOrg(args);   
}   
   
   
void Event_g3sg1(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.brokentime)   
        {   
            me.spread.spreadvar = 0.55 + (0.3 * (me.spread.gtime - me.spread.brokentime));   
   
            if (me.spread.spreadvar > 0.98)   
                me.spread.spreadvar = 0.98f;   
        }   
   
        me.spread.recoil++;   
        me.spread.brokentime = me.spread.gtime;   
        me.spread.firing = true;   
    }   
   
    g3sg1Org(args);   
}   
   
   
void Event_sg550(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.brokentime)   
        {   
            me.spread.spreadvar = 0.65 + (0.35 * (me.spread.gtime - me.spread.brokentime));   
   
            if (me.spread.spreadvar > 0.98)   
                me.spread.spreadvar = 0.98f;   
        }   
   
        me.spread.recoil++;   
        me.spread.brokentime = me.spread.gtime;   
        me.spread.firing = true;   
    }   
   
    sg550Org(args);   
}   
   
   
void Event_glock18(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.prevtime)   
        {   
            me.spread.spreadvar = me.spread.spreadvar - (0.275 * (0.325 - (me.spread.gtime - me.spread.prevtime)));   
   
            if (me.spread.spreadvar > 0.9)   
                me.spread.spreadvar = 0.9f;   
            else if (me.spread.spreadvar < 0.6)   
                me.spread.spreadvar = 0.6f;   
        }   
   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    glock18Org(args);   
}   
   
   
void Event_m249(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
           
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * me.spread.recoil / 175 + 0.4;   
   
        if (me.spread.spreadvar > 0.9)   
            me.spread.spreadvar = 0.9f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    m249Org(args);   
}   
   
   
void Event_m3(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
        me.spread.firing = true;   
    }   
   
    m3Org(args);   
}   
   
   
void Event_m4a1(struct event_args_s *args)   
{   
    //float spread[2], x, y;   
    //char buffer[256];   
   
    if (args->entindex == me.ent->index)   
    {   
   
        me.spread.recoil++;   
        me.spread.firing = true;   
           
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * me.spread.recoil / 220 + 0.3;   
   
        if (me.spread.spreadvar > 1)   
            me.spread.spreadvar = 1.0f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    m4a1Org(args);   
}   
   
   
void Event_mac10(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
           
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * me.spread.recoil / 200 + 0.6;   
   
        if (me.spread.spreadvar > 1.65)   
            me.spread.spreadvar = 1.65f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    mac10Org(args);   
}   
   
   
void Event_p90(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
   
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil / 175 + 0.45;   
   
        if (me.spread.spreadvar > 1)   
            me.spread.spreadvar = 1.0f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    p90Org(args);   
}   
   
   
void Event_p228(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.prevtime)   
        {   
            me.spread.spreadvar = me.spread.spreadvar - (0.3 * (0.325 - (me.spread.gtime - me.spread.prevtime)));   
   
            if (me.spread.spreadvar > 0.9)   
                me.spread.spreadvar = 0.9f;   
            else if (me.spread.spreadvar < 0.6)   
                me.spread.spreadvar = 0.6f;   
        }   
   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    p228Org(args);   
}   
   
   
void Event_awp(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
        me.spread.firing = true;   
    }   
   
    awpOrg(args);   
}   
   
   
void Event_scout(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
        me.spread.firing = true;   
    }   
   
    scoutOrg(args);   
}   
   
   
void Event_sg552(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
           
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * me.spread.recoil / 220 + 0.3;   
   
        if (me.spread.spreadvar > 1)   
            me.spread.spreadvar = 1.0f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    sg552Org(args);   
}   
   
   
void Event_tmp(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
   
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil * me.spread.recoil / 200 + 0.55;   
   
        if (me.spread.spreadvar > 1.4)   
            me.spread.spreadvar = 1.4f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    tmpOrg(args);   
}   
   
   
void Event_fiveseven(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.prevtime)   
        {   
            me.spread.spreadvar = me.spread.spreadvar - (0.25 * (0.275 - (me.spread.gtime - me.spread.prevtime)));   
   
            if (me.spread.spreadvar > 0.92)   
                me.spread.spreadvar = 0.92f;   
            else if (me.spread.spreadvar < 0.725)   
                me.spread.spreadvar = 0.725f;   
        }   
   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    fivesevenOrg(args);   
}   
   
   
void Event_ump45(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.firing = true;   
   
        me.spread.spreadvar = me.spread.recoil * me.spread.recoil / 210 + 0.5;   
   
        if (me.spread.spreadvar > 1.0)   
            me.spread.spreadvar = 1.0f;   
   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    ump45Org(args);   
}   
   
   
void Event_xm1014(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
        me.spread.firing = true;   
    }   
   
    xm1014Org(args);   
}   
   
   
void Event_elite_left(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.prevtime)   
        {   
            me.spread.spreadvar = me.spread.spreadvar - (0.275 * (0.325 - (me.spread.gtime - me.spread.prevtime)));   
   
            if (me.spread.spreadvar > 0.88)   
                me.spread.spreadvar = 0.88f;   
            else if (me.spread.spreadvar < 0.55)   
                me.spread.spreadvar = 0.55f;   
        }   
   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    elite_leftOrg(args);   
}   
   
   
void Event_elite_right(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        if (me.spread.prevtime)   
        {   
            me.spread.spreadvar = me.spread.spreadvar - (0.275 * (0.325 - (me.spread.gtime - me.spread.prevtime)));   
   
            if (me.spread.spreadvar > 0.88)   
                me.spread.spreadvar = 0.88f;   
            else if (me.spread.spreadvar < 0.55)   
                me.spread.spreadvar = 0.55f;   
        }   
   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    elite_rightOrg(args);   
}   
   
   
void Event_knife(struct event_args_s *args)   
{   
    if (args->entindex == me.ent->index)   
    {   
        me.spread.recoil++;   
        me.spread.prevtime = me.spread.gtime;   
    }   
   
    knifeOrg(args);   
}   
   
   
void Event_decal_reset(struct event_args_s *args)   
{   
    decal_resetOrg(args);   
}   
   
   
void Event_createsmoke(struct event_args_s *args)   
{   
    createsmokeOrg(args);   
}   
   
   
//#include <fstream>   
//std::ofstream outstream("D:\\jumpshootdebug.txt");   
   
void PlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)   
{   
   
    gEngfuncs.pfnPlaybackEvent(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);   
}   
   
   
void HookEvent(char *name, void (*pfnEvent)(struct event_args_s *args))   
{   
    gEngfuncs.pfnHookEvent(name, pfnEvent);   
   
    if (!strcmp(name, "events/usp.sc"))    
    {   
        uspOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_usp);   
    }   
    else if (!strcmp(name, "events/mp5n.sc"))    
    {   
        mp5nOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_mp5n);   
    }   
    else if (!strcmp(name, "events/ak47.sc"))    
    {   
        ak47Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_ak47);   
    }   
    else if (!strcmp(name, "events/aug.sc"))    
    {   
        augOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_aug);   
    }   
    else if (!strcmp(name, "events/deagle.sc"))    
    {   
        deagleOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_deagle);   
    }   
    else if (!strcmp(name, "events/g3sg1.sc"))    
    {   
        g3sg1Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_g3sg1);   
    }   
    else if (!strcmp(name, "events/sg550.sc"))    
    {   
        sg550Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_sg550);   
    }   
    else if (!strcmp(name, "events/glock18.sc"))    
    {   
        glock18Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_glock18);   
    }   
    else if (!strcmp(name, "events/m249.sc"))    
    {   
        m249Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_m249);   
    }   
    else if (!strcmp(name, "events/m3.sc"))    
    {   
        m3Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_m3);   
    }   
    else if (!strcmp(name, "events/m4a1.sc"))    
    {   
        m4a1Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_m4a1);   
    }   
    else if (!strcmp(name, "events/mac10.sc"))    
    {   
        mac10Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_mac10);   
    }   
    else if (!strcmp(name, "events/p90.sc"))    
    {   
        p90Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_p90);   
    }   
    else if (!strcmp(name, "events/p228.sc"))    
    {   
        p228Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_p228);   
    }   
    else if (!strcmp(name, "events/awp.sc"))    
    {   
        awpOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_awp);   
    }   
    else if (!strcmp(name, "events/scout.sc"))    
    {   
        scoutOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_scout);   
    }   
    else if (!strcmp(name, "events/sg552.sc"))    
    {   
        sg552Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_sg552);   
    }   
    else if (!strcmp(name, "events/tmp.sc"))    
    {   
        tmpOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_tmp);   
    }   
    else if (!strcmp(name, "events/fiveseven.sc"))    
    {   
        fivesevenOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_fiveseven);   
    }   
    else if (!strcmp(name, "events/ump45.sc"))    
    {   
        ump45Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_ump45);   
    }   
    else if (!strcmp(name, "events/xm1014.sc"))    
    {   
        xm1014Org = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_xm1014);   
    }   
    else if (!strcmp(name, "events/elite_left.sc"))    
    {   
        elite_leftOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_elite_left);   
    }   
    else if (!strcmp(name, "events/elite_right.sc"))    
    {   
        elite_rightOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_elite_right);   
    }   
    else if (!strcmp(name, "events/knife.sc"))    
    {   
        knifeOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_knife);   
    }   
    else if (!strcmp(name, "events/decal_reset.sc"))    
    {   
        decal_resetOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_decal_reset);   
    }   
    else if (!strcmp(name, "events/createsmoke.sc"))    
    {   
        createsmokeOrg = pfnEvent;   
        gEngfuncs.pfnHookEvent(name, Event_createsmoke);   
    }   
}   