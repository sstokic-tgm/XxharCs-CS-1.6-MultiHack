#pragma warning (disable:4786)
#include <Windows.h>
#include <vector>
#include "gamehooking.h"
#include "playeritems.h"
#include "stringfinder.h"
#include <map>
#include <fstream>
#define WEAPONLIST_GALIL 14
#define WEAPONLIST_FAMAS 15

PlayerItems playerItems;

//===================================================================================
typedef map<int,int> MapIntInt;
struct PrivateData{
	MapIntInt     findByID;
	StringFinder  findByName;
};
#define INDEXFIND_BY_ID   (((PrivateData*)my_data)->findByID)
#define INDEXFIND_BY_NAME (((PrivateData*)my_data)->findByName)

//===================================================================================
PlayerItems::PlayerItems()
{
	my_data = new PrivateData;
	slot1Index=-1;
	slot2Index=-1;
	currentSlot=-1;
}

//===================================================================================
PlayerItems::~PlayerItems()
{
    delete (PrivateData*)my_data;
}

//===================================================================================
bool PlayerItems::canSwitch()
{
	if(currentSlot==0){
		if(slot2Index!=-1){
			HlWeaponInfo& Pistol = weaponList[slot2Index];
			return (Pistol.iClip>0);
		}
	}
	return false;
}


void PlayerItems::SetWeaponData(struct HlWeaponInfo *weapon)
{
	switch (weapon->iId)
	{
		case WEAPONLIST_P228:
			weapon->ClipCap = CLIPCAP_P228;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 4096.0f;
			weapon->wallpierce1 = 0.8f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 32;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_P228;
			break;
		case WEAPONLIST_UNKNOWN1:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 0.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_NONE;
			break;
		case WEAPONLIST_SCOUT:
			weapon->ClipCap = CLIPCAP_SCOUT;
			weapon->penetrate = WALL_PEN2;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.98f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 75;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_SCOUT;
			break;
		case WEAPONLIST_HEGRENADE:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 0.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_NONE;
			break;
		case WEAPONLIST_XM1014:
			weapon->ClipCap = CLIPCAP_XM1014;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 3048.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_XM1014;
			break;
		case WEAPONLIST_C4:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 0.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_NONE;
			break;
		case WEAPONLIST_MAC10:
			weapon->ClipCap = CLIPCAP_MAC10;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.82f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 29;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_MAC10;
			break;
		case WEAPONLIST_AUG:
			weapon->ClipCap = CLIPCAP_AUG;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.96f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 32;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_AUG;
			break;
		case WEAPONLIST_SMOKEGRENADE:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 0.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_NONE;
			break;
		case WEAPONLIST_ELITE:
			weapon->ClipCap = CLIPCAP_ELITE;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.75f; // left
			weapon->wallpierce2 = 0.75f; // right
			weapon->damage1 = 20; // left
			weapon->damage2 = 27; // right
			weapon->bullettype = BULLET_ELITE;
			break;
		case WEAPONLIST_FIVESEVEN:
			weapon->ClipCap = CLIPCAP_FIVESEVEN;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 4096.0f;
			weapon->wallpierce1 = 0.885f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 14;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_FIVESEVEN;
			break;
		case WEAPONLIST_UMP45:
			weapon->ClipCap = CLIPCAP_UMP45;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.82f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 30;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_UMP45;
			break;
		case WEAPONLIST_SG550:
			weapon->ClipCap = CLIPCAP_SG550;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.98f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 40;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_SG550;
			break;
		case WEAPONLIST_USP:
			weapon->ClipCap = CLIPCAP_USP;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 4096.0f;
			weapon->wallpierce1 = 0.79f; // unsilenced
			weapon->wallpierce2 = 0.79f; // silenced
			weapon->damage1 = 34; // unsilenced
			weapon->damage2 = 30; // silenced
			weapon->bullettype = BULLET_USP;
			break;
		case WEAPONLIST_GLOCK18:
			weapon->ClipCap = CLIPCAP_GLOCK18;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.75f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 20;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_GLOCK18;
			break;
		case WEAPONLIST_AWP:
			weapon->ClipCap = CLIPCAP_AWP;
			weapon->penetrate = WALL_PEN2;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.99f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 115;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_AWP;
			break;
		case WEAPONLIST_MP5:
			weapon->ClipCap = CLIPCAP_MP5;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.84f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 26;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_MP5;
			break;
		case WEAPONLIST_M249:
			weapon->ClipCap = CLIPCAP_M249;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.97f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 32;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_M249;
			break;
		case WEAPONLIST_M3:
			weapon->ClipCap = CLIPCAP_M3;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 3000.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_M3;
			break;
		case WEAPONLIST_M4A1:
			weapon->ClipCap = CLIPCAP_M4A1;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.97f; // unsilenced
			weapon->wallpierce2 = 0.95f; // silenced
			weapon->damage1 = 32; // unsilenced
			weapon->damage2 = 33; // silenced
			weapon->bullettype = BULLET_M4A1;
			break;
		case WEAPONLIST_TMP:
			weapon->ClipCap = CLIPCAP_TMP;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.85f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 20;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_TMP;
			break;
		case WEAPONLIST_G3SG1:
			weapon->ClipCap = CLIPCAP_G3SG1;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.98f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 60;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_G3SG1;
			break;
		case WEAPONLIST_FLASHBANG:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 0.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_NONE;
			break;
		case WEAPONLIST_DEAGLE:
			weapon->ClipCap = CLIPCAP_DEAGLE;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 4096.0f;
			weapon->wallpierce1 = 0.81f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 54;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_DEAGLE;
			break;
		case WEAPONLIST_SG552:
			weapon->ClipCap = CLIPCAP_SG552;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.955f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 33;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_SG552;
			break;
		case WEAPONLIST_AK47:
			weapon->ClipCap = CLIPCAP_AK47;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.98f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 36;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_AK47;
			break;
		case WEAPONLIST_KNIFE:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 0.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_NONE;
			break;
		case WEAPONLIST_P90:
			weapon->ClipCap = CLIPCAP_P90;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.885f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 21;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_P90;
			break;
		case WEAPONLIST_FAMAS:
			weapon->ClipCap = CLIPCAP_M4A1;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.98f; // unsilenced
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 34; // unsilenced
			weapon->damage2 =  0;
			weapon->bullettype = BULLET_M4A1;
			break;
		case WEAPONLIST_GALIL:
			weapon->ClipCap = CLIPCAP_M4A1;
			weapon->penetrate = WALL_PEN1;
			weapon->distance = 8192.0f;
			weapon->wallpierce1 = 0.98f; // unsilenced
			weapon->wallpierce2 = 0.00f; 
			weapon->damage1 = 30; // unsilenced
			weapon->damage2 =  0; 
			weapon->bullettype = BULLET_AK47;
			break;
		default:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			weapon->distance = 0.0f;
			weapon->wallpierce1 = 0.0f;
			weapon->wallpierce2 = 0.0f;
			weapon->damage1 = 0;
			weapon->damage2 = 0;
			weapon->bullettype = BULLET_NONE;
			break;
	}
}


//===================================================================================
void PlayerItems::msgWeaponList(int iSize, void* pbuf)
{
	HlWeaponInfo Weapon;
	BEGIN_READ( pbuf, iSize );
	char* name = READ_STRING();
	if(strstr(name,"weapon_")==name) { name += 7;} 
	strcpy( Weapon.szName, name );
	Weapon.iAmmoType = (int)READ_CHAR();	
	
	Weapon.iMax1 = READ_BYTE();
	if (Weapon.iMax1 == 255)
		Weapon.iMax1 = -1;
	Weapon.iAmmo2Type = READ_CHAR();
	Weapon.iMax2 = READ_BYTE();
	if (Weapon.iMax2 == 255)
		Weapon.iMax2 = -1;
	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
	Weapon.iId = READ_CHAR();
	Weapon.iFlags = READ_BYTE();
	Weapon.iClip = 0;

	SetWeaponData(&Weapon);

	MapIntInt::iterator foundpos = INDEXFIND_BY_ID.find( Weapon.iId );
	typedef MapIntInt::value_type Entry;
	if(foundpos==(INDEXFIND_BY_ID.end()))
	{
		int index = weaponList.size();
		weaponList.push_back(Weapon);
		INDEXFIND_BY_ID.insert(Entry(Weapon.iId,index));
		INDEXFIND_BY_NAME.add(Weapon.szName,index);
	} else {
		int index = foundpos->second;
		weaponList[index]=Weapon;
	}	
}

//===================================================================================
void PlayerItems::msgCurWeapon (int iStat, int iId, int iClip )
{
	if(iStat){ currentSlot = -1; }
	MapIntInt::iterator foundpos = INDEXFIND_BY_ID.find( iId );
	if(foundpos==(INDEXFIND_BY_ID.end())) { return; }
	int index = foundpos->second;
	HlWeaponInfo& Weapon = weaponList[index];
	Weapon.iClip = iClip;
	if(Weapon.iSlot==0){ slot1Index = index; }
	if(Weapon.iSlot==1){ slot2Index = index; }
	if(iStat){ currentSlot = Weapon.iSlot; }
}

//===================================================================================
bool PlayerItems::hasWeapon(const char* name)
{
	bool found = INDEXFIND_BY_NAME.find(name);
	if(!found){ return false; }
	int index = INDEXFIND_BY_NAME.num;
	return (slot1Index==index || slot2Index==index);
}

//===================================================================================

char* PlayerItems::getNamebyId(int id)
{
	MapIntInt::iterator foundpos = INDEXFIND_BY_ID.find( id );
	if(foundpos==(INDEXFIND_BY_ID.end())) { return "none"; }
	int index = foundpos->second;
	return weaponList[index].szName;

}


int PlayerItems::GetCurWeaponindex(void)
{
	int index;

	if (currentSlot == -1)
		return -1;

	if (currentSlot == 0)
		index = slot1Index;
	else if (currentSlot == 1)
		index = slot2Index;
	else
		return -1;

	return index;
}


int PlayerItems::CurrentID(void)
{
	int index;

	index = GetCurWeaponindex();

	if (index == -1)
		return -1;

	return weaponList[index].iId;
}


int PlayerItems::CurPenetration(void)
{
	int index;

	index = GetCurWeaponindex();

	if (index == -1)
		return 0;

	return weaponList[index].penetrate;
}


float PlayerItems::CurDistance(void)
{
	int index;

	index = GetCurWeaponindex();

	if (index == -1)
		return 0.0f;

	return weaponList[index].distance;
}


int PlayerItems::CurBulletType(void)
{
	int index;

	index = GetCurWeaponindex();

	if (index == -1)
		return BULLET_NONE;

	return weaponList[index].bullettype;
}

/*
float PlayerItems::CurWallPierce(void)
{
	int index;

	index = GetCurWeaponindex();

	if (index == -1)
		return 0.0f;

	if (weaponList[index].iId == WEAPONLIST_M4A1)
	{
		if (me.spread.WeaponState & M4A1_SILENCER) // note me should not be in here but as long as me.spread.WeaponState has the data for the current weapon it is ok
			return weaponList[index].wallpierce2;
		else
			return weaponList[index].wallpierce1;
	}

	return weaponList[index].wallpierce1;
}*/

bool IsCurWeaponSilenced(void);
bool IsLeftElite(void);

float PlayerItems::CurWallPierce(void)
{
	int index;

	index = GetCurWeaponindex();

	if (index == -1)
		return 0.0f;

	if (weaponList[index].iId == WEAPONLIST_M4A1)
	{
		if (IsCurWeaponSilenced()) // note me should not be in here but as long as me.spread.WeaponState has the data for the current weapon it is ok
			return weaponList[index].wallpierce2;
		else
			return weaponList[index].wallpierce1;
	}

	return weaponList[index].wallpierce1;
}
int PlayerItems::CurDamage(void)
{
	int index;

	index = GetCurWeaponindex();

	if (index == -1)
		return 0;

	if (weaponList[index].iId == WEAPONLIST_M4A1)
	{
		if (IsCurWeaponSilenced())
			return weaponList[index].damage2;
		else
			return weaponList[index].damage1;
	}

	if (weaponList[index].iId == WEAPONLIST_ELITE)
	{
		if (IsLeftElite())
			return weaponList[index].damage1;
		else
			return weaponList[index].damage2;
	}

	if (weaponList[index].iId == WEAPONLIST_USP)
	{
		if (IsCurWeaponSilenced())
			return weaponList[index].damage2;
		else
			return weaponList[index].damage1;
	}

	return weaponList[index].damage1;
}