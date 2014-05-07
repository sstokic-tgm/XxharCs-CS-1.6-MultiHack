#ifndef __WEAPONSLIST_H__
#define __WEAPONSLIST_H__

#include <vector>

//#include "weaponinfo.h"


#define WEAPONLIST_P228		1
#define	WEAPONLIST_UNKNOWN1	2
#define	WEAPONLIST_SCOUT	3
#define	WEAPONLIST_HEGRENADE 4
#define	WEAPONLIST_XM1014 5
#define	WEAPONLIST_C4 6
#define	WEAPONLIST_MAC10 7
#define	WEAPONLIST_AUG 8
#define	WEAPONLIST_SMOKEGRENADE 9
#define	WEAPONLIST_ELITE 10
#define	WEAPONLIST_FIVESEVEN 11
#define	WEAPONLIST_UMP45 12
#define	WEAPONLIST_SG550 13
#define	WEAPONLIST_UNKNOWN2 14
#define	WEAPONLIST_UNKNOWN3 15
#define	WEAPONLIST_USP 16
#define	WEAPONLIST_GLOCK18 17
#define	WEAPONLIST_AWP 18
#define	WEAPONLIST_MP5 19
#define	WEAPONLIST_M249 20
#define	WEAPONLIST_M3 21
#define	WEAPONLIST_M4A1 22
#define	WEAPONLIST_TMP 23
#define	WEAPONLIST_G3SG1 24
#define	WEAPONLIST_FLASHBANG 25
#define	WEAPONLIST_DEAGLE 26
#define	WEAPONLIST_SG552 27
#define	WEAPONLIST_AK47 28
#define	WEAPONLIST_KNIFE 29
#define	WEAPONLIST_P90 30


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

#define USP_SILENCER (1 << 0)
#define GLOCK18_BURST (1 << 1)
#define M4A1_SILENCER (1 << 2)
#define ELITE_LEFT (1 << 4)

#define MAX_WEAPON_NAME 128

struct Weapon_List
{
	char name[MAX_WEAPON_NAME];
	int len;
	char command[MAX_WEAPON_NAME];
	int AmmoType;
	int Ammo2Type;
	int Max1;
	int Max2;
	int Slot;
	int SlotPos;
	int Id;
	int Flags;
	int ClipCap;	// Number of bullets the clip can hold
	int CAmmo;		// Current ammo in clip
	int XAmmo;		// Extra ammo
	bool Active;	// True if we have this weapon
	bool CurWeapon; // True if this is the current weapon
	int penetrate;
	weapon_data_t weapondata;
};


Weapon_List *GetCurWeapon(void);

const char *GetPriWeaponName(void); // Returns the name of the primary weapon
const char *GetSecWeaponName(void); // Returns the name of the secondary weapon

const char *GetCurWeaponName(void); // Returns current weapon name

int GetCurWeaponAmmo(void);

int GetPrimaryAmmo(void); // Returns the amount of ammo in the primary weapon's clip
int GetSecondaryAmmo(void); // Returns the amount of ammo in the secondary weapon's clip

int GetPrimaryAmmoX(void); // Returns the amount of extra ammo for the primary weapon
int GetSecondaryAmmoX(void); // Returns the amount of extra ammo for the secondary weapon

int GetWeaponIndexByID(int WeaponID);
int GetCurWeaponID(void);

bool HasPriWeapon(void); // Returns true if we have a primary weapon
bool HasSecWeapon(void); // Returns true if we have a secondary weapon

bool IsCurWeaponPri(void); // Returns true if the current weapon is the primary
bool IsCurWeaponSec(void); // Returns true if the current weapon is the secondary

bool IsCurWeaponGun(void); // Returns true if the current weapon is either a primary or secondary weapon
bool IsCurWeaponKnife(void); // Returns true if the current weapon is a knife
bool IsCurWeaponNonAttack(void); // Returns true if the current weapon is not a gun or a knife
bool IsCurWeaponNade(void);
bool IsCurWeaponC4(void);

bool CurWeaponHasAmmo(void); // Returns true if the current weapon has ammo
bool SecWeaponHasAmmo(void); // Returns true if the secondary weapon has ammo
bool BackupWeaponHasAmmo(void); // Returns true if the backup weapon has ammo

bool CanCurWeaponAttack(void);

bool IsReloading(void);

bool HasWeaponName(const char *weaponname); // Returns true if we curently have this weapon

const char *GetWeaponNameByID(int Id);

const char *GetWeaponName(int weaponmodel);

int GetCurWeaponId(void);
int GetPriWeaponId(void);
int GetSecWeaponId(void);

const char *GetPriWeaponCmd(void);
const char *GetSecWeaponCmd(void);

int GetCurRecoil(void);

int GetClipCap(int ID);

int GetCurPenetration(void);

bool IsCurWeaponSilenced(void);
bool IsCurWeaponInBurst(void);
// MsgHook Functions
void WeaponListCurWeapon(int CurWeapon, int WeaponID, int Ammo);
void WeaponListAmmoX(int ID, int Count);
void WeaponListUpdate(int WeaponBits);
void WeaponListAdd(const char *weaponname, int ammo1type, int max1, int ammo2type, int max2, int slot, int slotpos, int id, int flags);

extern std::vector<Weapon_List> WeaponList;

#endif
