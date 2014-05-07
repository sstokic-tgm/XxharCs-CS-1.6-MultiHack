#pragma warning (disable:4786)
#include <Windows.h>
#include <string.h>
#include "gamehooking.h"
#include "weaponlist.h"

using namespace std;

vector<Weapon_List> WeaponList;


bool IsCurWeaponSilenced(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
	{
		if (tmpWeapon->Id == WEAPONLIST_M4A1 && tmpWeapon->weapondata.m_iWeaponState & M4A1_SILENCER)
			return true;

		if (tmpWeapon->Id == WEAPONLIST_USP && tmpWeapon->weapondata.m_iWeaponState & USP_SILENCER)
			return true;
	}

	return false;
}
bool IsLeftElite(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
	{
		if (tmpWeapon->Id == WEAPONLIST_M4A1 && tmpWeapon->weapondata.m_iWeaponState & ELITE_LEFT)
			return true;
	}

	return false;
}

bool IsCurWeaponInBurst(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
	{
		if (tmpWeapon->Id == WEAPONLIST_GLOCK18 && tmpWeapon->weapondata.m_iWeaponState & GLOCK18_BURST)
			return true;
	}

	return false;
}


int GetCurPenetration(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
		return tmpWeapon->penetrate;

	return WALL_PEN0;
}


Weapon_List *GetCurWeapon(void)
{
	for (int i=0;i<WeaponList.size();i++)
	{
		if (WeaponList[i].Active && WeaponList[i].CurWeapon)
			return &WeaponList[i];
	}
	return NULL;
}


int GetCurWeaponAmmo(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
		return tmpWeapon->CAmmo;

	return 0;
}


int GetCurRecoil(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
		return tmpWeapon->weapondata.m_fInZoom;

	return 0;
}


bool IsReloading(void)
{
	Weapon_List *tmpweapon;

	tmpweapon = GetCurWeapon();
	if (tmpweapon && tmpweapon->weapondata.m_fInReload)
		return true;

	return false;
}


bool CanCurWeaponAttack(void)
{
	Weapon_List *tmpweapon;

	tmpweapon = GetCurWeapon();
	if (tmpweapon && tmpweapon->weapondata.m_flNextPrimaryAttack <= 0)
		return true;

	return false;
}


static Weapon_List *GetSecWeapon(void)
{
	int i;

	for (i=0;i<WeaponList.size();i++)
	{
		if (WeaponList[i].Active && WeaponList[i].Slot == 1)
			return &WeaponList[i];
	}

	return NULL;
}


static Weapon_List *GetPriWeapon(void)
{
	int i;

	for (i=0;i<WeaponList.size();i++)
	{
		if (WeaponList[i].Active && WeaponList[i].Slot == 0)
			return &WeaponList[i];
	}

	return NULL;
}


int GetWeaponIndexByID(int WeaponID)
{
	int i;

	for (i=0;i<WeaponList.size();i++)
	{
		if (WeaponList[i].Id == WeaponID)
			return i;
	}

	return -1;
}


int GetCurWeaponId(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
		return tmpWeapon->Id;

	return -1;
}


int GetPriWeaponId(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetPriWeapon();
	if (tmpWeapon)
		return tmpWeapon->Id;

	return -1;
}


int GetSecWeaponId(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetSecWeapon();
	if (tmpWeapon)
		return tmpWeapon->Id;

	return -1;
}


const char *GetPriWeaponCmd(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetPriWeapon();
	if (tmpWeapon)
		return tmpWeapon->command;

	return NULL;
}


const char *GetSecWeaponCmd(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetSecWeapon();
	if (tmpWeapon)
		return tmpWeapon->command;

	return NULL;
}


const char *GetPriWeaponName(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetPriWeapon();
	if (tmpWeapon)
		return tmpWeapon->name;

	return NULL;
}


const char *GetSecWeaponName(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetSecWeapon();
	if (tmpWeapon)
		return tmpWeapon->name;

	return NULL;
}


bool HasPriWeapon(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetPriWeapon();
	if (tmpWeapon)
		return true;

	return false;
}


bool HasSecWeapon(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetSecWeapon();
	if (tmpWeapon)
		return true;

	return false;
}


int GetPrimaryAmmoX(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetPriWeapon();
	if (tmpWeapon)
		return tmpWeapon->XAmmo;

	return -1;
}


int GetSecondaryAmmoX(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetSecWeapon();
	if (tmpWeapon)
		return tmpWeapon->XAmmo;

	return -1;
}


int GetPrimaryAmmo(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetPriWeapon();
	if (tmpWeapon)
		return tmpWeapon->CAmmo;

	return -1;
}


int GetSecondaryAmmo(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetSecWeapon();
	if (tmpWeapon)
		return tmpWeapon->CAmmo;

	return -1;
}


bool IsCurWeaponPri(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetPriWeapon();
	if (tmpWeapon && tmpWeapon->CurWeapon)
		return true;

	return false;
}


bool IsCurWeaponSec(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetSecWeapon();
	if (tmpWeapon && tmpWeapon->CurWeapon)
		return true;

	return false;
}


bool IsCurWeaponGun(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon && (tmpWeapon->Slot == 0 || tmpWeapon->Slot == 1))
		return true;

	return false;
}


bool CurWeaponHasAmmo(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon && tmpWeapon->CAmmo > 0)
		return true;

	return false;
}


bool IsCurWeaponKnife(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon && tmpWeapon->Id == WEAPONLIST_KNIFE)
		return true;

	return false;
}


bool IsCurWeaponNonAttack(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon && (tmpWeapon->Id == WEAPONLIST_HEGRENADE || tmpWeapon->Id == WEAPONLIST_FLASHBANG || tmpWeapon->Id == WEAPONLIST_C4 || tmpWeapon->Id == WEAPONLIST_SMOKEGRENADE))
		return true;

	return false;
}


bool IsCurWeaponNade(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon && (tmpWeapon->Id == WEAPONLIST_HEGRENADE || tmpWeapon->Id == WEAPONLIST_FLASHBANG || tmpWeapon->Id == WEAPONLIST_SMOKEGRENADE))
		return true;

	return false;
}


bool IsCurWeaponC4(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon && tmpWeapon->Id == WEAPONLIST_C4)
		return true;

	return false;
}


const char *GetCurWeaponName(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
		return tmpWeapon->name;

	return NULL;
}


int GetCurWeaponID(void)
{
	Weapon_List *tmpWeapon;

	tmpWeapon = GetCurWeapon();
	if (tmpWeapon)
		return tmpWeapon->Id;

	return 0;
}


bool HasWeaponName(const char *weaponname)
{
	int i, len;

	len = strlen(weaponname);

	for (i=0;i<WeaponList.size();i++)
	{
		if (WeaponList[i].Active && len == WeaponList[i].len && !memcmp(WeaponList[i].name, weaponname, len))
			return true;
	}

	return false;
}


void WeaponListUpdate(int WeaponBits)
{
	int i;
	static int OldWeaponBits = 0;

	if (WeaponBits != OldWeaponBits)
	{
		OldWeaponBits = WeaponBits;

		for (i=0;i<WeaponList.size();i++)
		{
			if (WeaponBits & (1 << WeaponList[i].Id))
			{
				WeaponList[i].Active = true;
			}
			else
			{
				WeaponList[i].Active = false;
				WeaponList[i].CAmmo = 0;
			}
		}
	}
}


void WeaponListCurWeapon(int CurWeapon, int WeaponID, int Ammo)
{
	int index;

	index = GetWeaponIndexByID(WeaponID);
	if (index != -1)
		WeaponList[index].CAmmo = Ammo;
}


void WeaponListAmmoX(int ID, int Count)
{
	int i;

	for (i=0;i<WeaponList.size();i++)
	{
		if (WeaponList[i].AmmoType == ID)
			WeaponList[i].XAmmo = Count;
	}
}


void SetWeaponData(struct Weapon_List *weapon)
{
	switch (weapon->Id)
	{
		case WEAPONLIST_P228:
			weapon->ClipCap = CLIPCAP_P228;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_UNKNOWN1:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_SCOUT:
			weapon->ClipCap = CLIPCAP_SCOUT;
			weapon->penetrate = WALL_PEN2;
			break;
		case WEAPONLIST_HEGRENADE:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_XM1014:
			weapon->ClipCap = CLIPCAP_XM1014;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_C4:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_MAC10:
			weapon->ClipCap = CLIPCAP_MAC10;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_AUG:
			weapon->ClipCap = CLIPCAP_AUG;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_SMOKEGRENADE:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_ELITE:
			weapon->ClipCap = CLIPCAP_ELITE;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_FIVESEVEN:
			weapon->ClipCap = CLIPCAP_FIVESEVEN;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_UMP45:
			weapon->ClipCap = CLIPCAP_UMP45;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_SG550:
			weapon->ClipCap = CLIPCAP_SG550;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_UNKNOWN2:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_UNKNOWN3:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_USP:
			weapon->ClipCap = CLIPCAP_USP;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_GLOCK18:
			weapon->ClipCap = CLIPCAP_GLOCK18;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_AWP:
			weapon->ClipCap = CLIPCAP_AWP;
			weapon->penetrate = WALL_PEN2;
			break;
		case WEAPONLIST_MP5:
			weapon->ClipCap = CLIPCAP_MP5;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_M249:
			weapon->ClipCap = CLIPCAP_M249;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_M3:
			weapon->ClipCap = CLIPCAP_M3;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_M4A1:
			weapon->ClipCap = CLIPCAP_M4A1;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_TMP:
			weapon->ClipCap = CLIPCAP_TMP;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_G3SG1:
			weapon->ClipCap = CLIPCAP_G3SG1;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_FLASHBANG:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_DEAGLE:
			weapon->ClipCap = CLIPCAP_DEAGLE;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_SG552:
			weapon->ClipCap = CLIPCAP_SG552;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_AK47:
			weapon->ClipCap = CLIPCAP_AK47;
			weapon->penetrate = WALL_PEN1;
			break;
		case WEAPONLIST_KNIFE:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
		case WEAPONLIST_P90:
			weapon->ClipCap = CLIPCAP_P90;
			weapon->penetrate = WALL_PEN0;
			break;
		default:
			weapon->ClipCap = 0;
			weapon->penetrate = WALL_PEN0;
			break;
	}
}


void WeaponListAdd(const char *weaponname, int ammo1type, int max1, int ammo2type, int max2, int slot, int slotpos, int id, int flags)
{
	struct Weapon_List dummy;
	int index, len;
	
	WeaponList.push_back(dummy);

	index = WeaponList.size() - 1;

	len = strlen(weaponname);
	if (len > MAX_WEAPON_NAME)
		return;

	memcpy(WeaponList[index].command, weaponname, len + 1);

	if (len > 7 && !memcmp(weaponname, "weapon_", 7))
	{
		weaponname = weaponname + 7;
		WeaponList[index].len = len - 7;
	}
	else
		WeaponList[index].len = len;

	// Catch the special case of mp5navy where the msghook name differs from model name mp5
	if (WeaponList[index].len == 7 && !memcmp(weaponname, "mp5navy", 7))
		WeaponList[index].len = 3;

	memcpy(WeaponList[index].name, weaponname, WeaponList[index].len);
	WeaponList[index].name[WeaponList[index].len] = 0;

	WeaponList[index].AmmoType = ammo1type;
	if (max1 == 255)
		WeaponList[index].Max1 = -1;
	else
		WeaponList[index].Max1 = max1;

	WeaponList[index].Ammo2Type = ammo2type;
	if (max2 == 255)
		WeaponList[index].Max2 = -1;
	else
		WeaponList[index].Max2 = max2;

	WeaponList[index].Slot = slot;
	WeaponList[index].SlotPos = slotpos;

	WeaponList[index].Id = id;
	WeaponList[index].Flags = flags;

	WeaponList[index].CAmmo = 0;
	WeaponList[index].XAmmo = 0;

	WeaponList[index].Active = false;
	WeaponList[index].CurWeapon = false;

	SetWeaponData(&WeaponList[index]);

	memset(&WeaponList[index].weapondata, 0, sizeof(weapon_data_t));
}

const char *GetWeaponNameByID(int Id)
{
	int index;

	index = GetWeaponIndexByID(Id);
	if (index != -1)
			return WeaponList[index].name;

	return NULL;
}

const char *GetWeaponName(int weaponmodel)
{
	return "unknown";
}