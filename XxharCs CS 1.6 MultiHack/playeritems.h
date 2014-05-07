#ifndef __PLAYERITEMS_H__
#define __PLAYERITEMS_H__

#define WEAPONLIST_P228 1
#define WEAPONLIST_UNKNOWN1 2
#define WEAPONLIST_SCOUT 3
#define WEAPONLIST_HEGRENADE 4
#define WEAPONLIST_XM1014 5
#define WEAPONLIST_C4 6
#define WEAPONLIST_MAC10 7
#define WEAPONLIST_AUG 8
#define WEAPONLIST_SMOKEGRENADE 9
#define WEAPONLIST_ELITE 10
#define WEAPONLIST_FIVESEVEN 11
#define WEAPONLIST_UMP45 12
#define WEAPONLIST_SG550 13
#define WEAPONLIST_UNKNOWN2 14
#define WEAPONLIST_UNKNOWN3 15
#define WEAPONLIST_USP 16
#define WEAPONLIST_GLOCK18 17
#define WEAPONLIST_AWP 18
#define WEAPONLIST_MP5 19
#define WEAPONLIST_M249 20
#define WEAPONLIST_M3 21
#define WEAPONLIST_M4A1 22
#define WEAPONLIST_TMP 23
#define WEAPONLIST_G3SG1 24
#define WEAPONLIST_FLASHBANG 25
#define WEAPONLIST_DEAGLE 26
#define WEAPONLIST_SG552 27
#define WEAPONLIST_AK47 28
#define WEAPONLIST_KNIFE 29
#define WEAPONLIST_P90 30

enum
{
	BULLETTYPE_0=0,
	BULLETTYPE_1=1,
	BULLETTYPE_2=2,
	BULLETTYPE_3=3,
	BULLETTYPE_4=4,
	BULLETTYPE_5=5,
	BULLETTYPE_6=6,
	BULLETTYPE_7=7,
	BULLETTYPE_8=8,
	BULLETTYPE_9=9,
	BULLETTYPE_10=10,
	BULLETTYPE_11=11,
	BULLETTYPE_12=12,
	BULLETTYPE_13=13,
	BULLETTYPE_14=14,
	BULLETTYPE_15=15
};

#define BULLET_NONE BULLETTYPE_0
#define BULLET_AK47 BULLETTYPE_11
#define BULLET_AUG BULLETTYPE_12
#define BULLET_AWP BULLETTYPE_10
#define BULLET_DEAGLE BULLETTYPE_13
#define BULLET_ELITE BULLETTYPE_1
#define BULLET_FIVESEVEN BULLETTYPE_15
#define BULLET_G3SG1 BULLETTYPE_11
#define BULLET_GLOCK18 BULLETTYPE_1
#define BULLET_M249 BULLETTYPE_12
#define BULLET_M4A1 BULLETTYPE_12
#define BULLET_MAC10 BULLETTYPE_9
#define BULLET_MP5 BULLETTYPE_1
#define BULLET_P228 BULLETTYPE_15
#define BULLET_P90 BULLETTYPE_14
#define BULLET_SCOUT BULLETTYPE_11
#define BULLET_SG550 BULLETTYPE_12
#define BULLET_SG552 BULLETTYPE_12
#define BULLET_TMP BULLETTYPE_1
#define BULLET_UMP45 BULLETTYPE_9
#define BULLET_USP BULLETTYPE_9
#define BULLET_M3 BULLETTYPE_0
#define BULLET_XM1014 BULLETTYPE_0


#define CLIPCAP_AK47 30
#define CLIPCAP_AUG 30
#define CLIPCAP_AWP 10
#define CLIPCAP_DEAGLE 7
#define CLIPCAP_ELITE 15
#define CLIPCAP_FIVESEVEN 20
#define CLIPCAP_G3SG1 20
#define CLIPCAP_GLOCK18 20
#define CLIPCAP_M249 100
#define CLIPCAP_M3 8
#define CLIPCAP_M4A1 30
#define CLIPCAP_MAC10 30
#define CLIPCAP_MP5 30
#define CLIPCAP_P228 13
#define CLIPCAP_P90 50
#define CLIPCAP_SCOUT 10
#define CLIPCAP_SG550 30
#define CLIPCAP_SG552 30
#define CLIPCAP_TMP 30
#define CLIPCAP_UMP45 25
#define CLIPCAP_USP 12
#define CLIPCAP_XM1014 7

#define WALL_PEN0 0
#define WALL_PEN1 1
#define WALL_PEN2 2

#define MAX_WEAPON_NAME 128
struct HlWeaponInfo
{
	char	szName[MAX_WEAPON_NAME];
	int		iAmmoType;
	int		iAmmo2Type;
	int		iMax1;
	int		iMax2;
	int		iSlot;
	int		iSlotPos;
	int		iFlags;
	int		iId;
	int		iClip;
	int ClipCap;
	int penetrate;
	float distance;
	int bullettype;
	float wallpierce1;
	float wallpierce2;
	int damage1;
	int damage2;
};

using namespace std;

class PlayerItems
{
public:
	void msgWeaponList(int iSize, void *pbuf );
	void msgCurWeapon (int iStat, int iId, int iClip );
	void clearWeaponSlots(){slot1Index=-1; slot2Index=-1;}
	char* getNamebyId(int id);
	bool canSwitch();
	int CurrentID(void);

	int CurDamage(void);
	float CurWallPierce(void);
	int CurBulletType(void);
	float CurDistance(void);
	int CurPenetration(void);

	char* getName1()
	{ 
		if(slot1Index==-1){ return "none"; }
		else              { return weaponList[slot1Index].szName; }
	}
	char* getName2()
	{ 
		if(slot2Index==-1){ return "none"; }
		else              { return weaponList[slot2Index].szName; }
	}   
	bool hasWeapon(const char* name);
	int slot1Index;
	int slot2Index;
	int currentSlot;
	int armor;
	int money;
protected:
	void* my_data;
private:
	vector<HlWeaponInfo> weaponList;
	void SetWeaponData(struct HlWeaponInfo *weapon);
	int GetCurWeaponindex(void);
public:
	int health;
	PlayerItems();
	~PlayerItems();
};

extern PlayerItems playerItems;

#endif