#include <windows.h>
#include <string>
#include <vector>
#include "gamehooking.h"
#include "perfectwall.h"
#include "playeritems.h"
#include "players.h"
#include "textures.h"

void MP_TraceLine(float *vecSrc, float *vecEnd, float *vecDir, int ignore, pmtrace_t *tr)
{
	float tmpSrc[3], length1, length2;

	if (vecSrc[0] == vecEnd[0] && vecSrc[1] == vecEnd[1] && vecSrc[2] == vecEnd[2])
	{
		memset(tr, 0, sizeof(pmtrace_t));

		tr->endpos[0] = vecEnd[0];
		tr->endpos[1] = vecEnd[1];
		tr->endpos[2] = vecEnd[2];

		tr->fraction = 1.0f;

		return;
	}

	//tmptr = gEngfuncs.PM_TraceLine(vecSrc, vecEnd, PM_TRACELINE_ANYVISIBLE, 2, ignore);

	//memcpy(tr, tmptr, sizeof(pmtrace_t));

	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_GLASS_IGNORE, -1, tr);

	//tmpSrc[0] = vecSrc[0];
	//tmpSrc[1] = vecSrc[1];
	//tmpSrc[2] = vecSrc[2];

	/*if (tr->inopen && !tr->startsolid)
	{
	tmpSrc[0] = tr->endpos[0] + vecDir[0];
	tmpSrc[1] = tr->endpos[1] + vecDir[1];
	tmpSrc[2] = tr->endpos[2] + vecDir[2];

	tmptr = gEngfuncs.PM_TraceLine(tmpSrc, vecEnd, PM_TRACELINE_PHYSENTSONLY, 2, ignore);

	memcpy(tr, tmptr, sizeof(pmtrace_t));

	if (!tr->allsolid)
	{
	tmpSrc[0] = vecEnd[0] - vecSrc[0];
	tmpSrc[1] = vecEnd[1] - vecSrc[1];
	tmpSrc[2] = vecEnd[2] - vecSrc[2];

	length1 = VectorLength(tmpSrc);

	tmpSrc[0] = tr->endpos[0] - vecSrc[0];
	tmpSrc[1] = tr->endpos[1] - vecSrc[1];
	tmpSrc[2] = tr->endpos[2] - vecSrc[2];

	length2 = VectorLength(tmpSrc);

	tr->fraction = length2 / length1;

	tr->startsolid = 0;

	tr->inopen = 1;
	}
	}
	else */
	if (tr->fraction == 0 && tr->startsolid && !tr->allsolid)
	{
		tmpSrc[0] = vecSrc[0];
		tmpSrc[1] = vecSrc[1];
		tmpSrc[2] = vecSrc[2];

		while (!tr->allsolid && tr->fraction == 0)
		{
			tmpSrc[0] += vecDir[0];
			tmpSrc[1] += vecDir[1];
			tmpSrc[2] += vecDir[2];

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(tmpSrc, vecEnd, PM_GLASS_IGNORE, -1, tr);

			//memset(tr, 0, sizeof(pmtrace_t));

			//tmptr = gEngfuncs.PM_TraceLine(tmpSrc, vecEnd, PM_TRACELINE_ANYVISIBLE, 2, ignore);

			//memcpy(tr, tmptr, sizeof(pmtrace_t));
		}

		if (!tr->allsolid && tr->fraction != 1.0)
		{
			tmpSrc[0] = vecEnd[0] - vecSrc[0];
			tmpSrc[1] = vecEnd[1] - vecSrc[1];
			tmpSrc[2] = vecEnd[2] - vecSrc[2];

			length1 = VectorLength(tmpSrc);

			tmpSrc[0] = tr->endpos[0] - vecSrc[0];
			tmpSrc[1] = tr->endpos[1] - vecSrc[1];
			tmpSrc[2] = tr->endpos[2] - vecSrc[2];

			length2 = VectorLength(tmpSrc);

			tr->fraction = length2 / length1;

			tr->startsolid = 1.0f;
		}
	}

	if (tr->allsolid)
		tr->fraction = 1.0f;
}


int CanPenetrate(const float *start, const float *end, float flDistance, int penetration, int bullettype, int iDamage, float wallpierce)
{
	float distfrac, damagemult, distancemax, vecSrc[3], vecEnd[3], vecDir[3], length, tmpVec[3];
	float tmplen, length1, length2;
	pmtrace_t tr;
	//struct cl_entity_s *pent;
	//physent_t *pent;
	int damage = iDamage, bulletvelocity, tmppen = penetration + 1;
	int y = 440;
	char texturetype;

	vecSrc[0] = start[0];
	vecSrc[1] = start[1];
	vecSrc[2] = start[2];

	vecEnd[0] = end[0];
	vecEnd[1] = end[1];
	vecEnd[2] = end[2];

	// damage gets a randomlong added to it in first switch

	switch (bullettype)
	{
	case BULLETTYPE_1:
		bulletvelocity = 21;
		distancemax = 800.0f;
		break;
	case BULLETTYPE_9:
		/*if (cvar.wallsensitivity == 0)
			damage -= 2;
		else if (cvar.wallsensitivity == 2)
			damage += 2;*/

		bulletvelocity = 15;
		distancemax = 500.0f;

		break;
	case BULLETTYPE_13:
		bulletvelocity = 30;
		distancemax = 1000.0f;
		break;
	case BULLETTYPE_11:
		/*if (cvar.wallsensitivity == 0)
			damage -= 2;
		else if (cvar.wallsensitivity == 2)
			damage += 2;*/

		bulletvelocity = 39;
		distancemax = 5000.0f;
		break;
	case BULLETTYPE_12:
		/*if (cvar.wallsensitivity == 0)
			damage -= 3;
		else if (cvar.wallsensitivity == 2)
			damage += 3;*/

		bulletvelocity = 35;
		distancemax = 4000.0f;
		break;
	case BULLETTYPE_10:
		/*if (cvar.wallsensitivity == 0)
			damage -= 4;
		else if (cvar.wallsensitivity == 2)
			damage += 4;*/

		bulletvelocity = 45;
		distancemax = 8000.0f;
		break;
	case BULLETTYPE_14:
		/*if (cvar.wallsensitivity == 0)
			damage -= 4;
		else if (cvar.wallsensitivity == 2)
			damage += 6;
		else
			damage += 1;*/

		bulletvelocity = 30;
		distancemax = 2000.0f;
		break;
	case BULLETTYPE_15:
		/*if (cvar.wallsensitivity == 0)
			damage -= 4;
		else if (cvar.wallsensitivity == 2)
			damage += 6;
		else
			damage += 1;*/

		bulletvelocity = 25;
		distancemax = 800.0f;
		break;
	default:
		bulletvelocity = 0;
		distancemax = 0.0f;
		break;
	}

	//DrawHudString(40,460,255,255,255, "distancemax: %f", distancemax);

	vecDir[0] = vecEnd[0] - vecSrc[0];
	vecDir[1] = vecEnd[1] - vecSrc[1];
	vecDir[2] = vecEnd[2] - vecSrc[2];

	length = VectorLength(vecDir);

	vecDir[0] /= length;
	vecDir[1] /= length;
	vecDir[2] /= length;

	//gEngfuncs.pfnAngleVectors(me.ViewAngles, forward, right, up);

	//vecDir[0] = forward[0];
	//vecDir[1] = forward[1];
	//vecDir[2] = forward[2];

	vecEnd[0] = vecDir[0] * flDistance + vecSrc[0];
	vecEnd[1] = vecDir[1] * flDistance + vecSrc[1];
	vecEnd[2] = vecDir[2] * flDistance + vecSrc[2];

	damagemult = 0.5f;

	while (tmppen)
	{
		//gDrawFilledBoxAtLocation(vecSrc, 0x00FF00FF, 2);

		MP_TraceLine(vecSrc, vecEnd, vecDir, -1, &tr);

		texturetype = GetTextureType(&tr, vecSrc, vecEnd);

		switch (texturetype)
		{
		case CHAR_TEX_CONCRETE:
			bulletvelocity *= 0.25;
			break;
		case CHAR_TEX_GRATE:
			bulletvelocity *= 0.5;
			damagemult = 0.4f;
			break;
		case CHAR_TEX_METAL:
			bulletvelocity *= 0.15;
			damagemult = 0.2f;
			break;
		case CHAR_TEX_COMPUTER:
			bulletvelocity *= 0.4;
			break;
		case CHAR_TEX_TILE:
			bulletvelocity *= 0.65;
			damagemult = 0.2f;
			break;
		case CHAR_TEX_VENT:
			bulletvelocity *= 0.5;
			damagemult = 0.45f;
			break;
		case CHAR_TEX_WOOD:
			damagemult = 0.6f;
		default:
			break;
		}

		//gDrawFilledBoxAtLocation(tr.endpos, 0xFFFFFFFF, 2);

		//DrawHudString(40,y+=20,255,255,255, "allsolid: %d, startsolid: %d, inopen: %d, fraction: %f", tr.allsolid, tr.startsolid, tr.inopen, tr.fraction);

		if (tr.fraction != 1.0)
		{
			//DrawHudString(40,y+=20,255,255,255, "damagemult: %f, bulletvelocity: %d", damagemult, bulletvelocity);

			tmppen--;

			tmpVec[0] = tr.endpos[0] - start[0];
			tmpVec[1] = tr.endpos[1] - start[1];
			tmpVec[2] = tr.endpos[2] - start[2];

			tmplen = VectorLength(tmpVec);

			if (tmplen >= length)
			{
				tmpVec[0] = end[0] - vecSrc[0];
				tmpVec[1] = end[1] - vecSrc[1];
				tmpVec[2] = end[2] - vecSrc[2];

				length1 = VectorLength(tmpVec);

				tmpVec[0] = vecEnd[0] - vecSrc[0];
				tmpVec[1] = vecEnd[1] - vecSrc[1];
				tmpVec[2] = vecEnd[2] - vecSrc[2];

				length2 = VectorLength(tmpVec);

				tr.fraction = length1 / length2;

				distfrac = flDistance * tr.fraction;

				damage *= pow(wallpierce, distfrac * 0.002f);

				return damage;
			}

			distfrac = flDistance * tr.fraction;

			damage *= pow(wallpierce, distfrac * 0.002f);

			if (distfrac > distancemax)
				tmppen = 0;

			// there is another test on tr.ent here but i don't see that is does anything important

			//pent = gEngfuncs.GetEntityByIndex(PM_GetPhysEntInfo(tr.ent));

			//pent = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

			//DrawHudString(40,y+=20,255,255,255, "tr.ent: %d, pent: %x, damage: %d, flDistance: %f, distfrac: %f, pen: %d, textype: %c", tr.ent, pent, damage, flDistance, distfrac, tmppen, texturetype);

			//if (pent)
			// DrawHudString(40,y+=20,255,255,255, "solid: %d", pent->solid);

			if (/*pent && pent->solid == SOLID_BSP && */tmppen)
			{
				//DrawHudString(40,y+=20,255,255,255, "one");

				vecSrc[0] = bulletvelocity * vecDir[0] + tr.endpos[0];
				vecSrc[1] = bulletvelocity * vecDir[1] + tr.endpos[1];
				vecSrc[2] = bulletvelocity * vecDir[2] + tr.endpos[2];

				flDistance = (flDistance - distfrac) * 0.5;

				vecEnd[0] = vecDir[0] * flDistance + vecSrc[0];
				vecEnd[1] = vecDir[1] * flDistance + vecSrc[1];
				vecEnd[2] = vecDir[2] * flDistance + vecSrc[2];

				distfrac = damage;

				//if (damage > 0 && pent->info == vPlayers[index].index)
				// return damage;

				damage = distfrac * damagemult;
			}
			else
			{
				//DrawHudString(40,y+=20,255,255,255, "two");

				vecSrc[0] = 42.0 * vecDir[0] + tr.endpos[0];
				vecSrc[1] = 42.0 * vecDir[1] + tr.endpos[1];
				vecSrc[2] = 42.0 * vecDir[2] + tr.endpos[2];

				flDistance = (flDistance - distfrac) * 0.75;

				vecEnd[0] = vecDir[0] * flDistance + vecSrc[0];
				vecEnd[1] = vecDir[1] * flDistance + vecSrc[1];
				vecEnd[2] = vecDir[2] * flDistance + vecSrc[2];

				//if (damage > 0 && pent && pent->info == vPlayers[index].index)
				// return damage;

				damage *= 0.75;
			}
		}
		else
			tmppen = 0;

		//if (damage > 0 && tr.ent > 0 && tr.ent < 33)
		// return damage;
	}

	return 0;
}