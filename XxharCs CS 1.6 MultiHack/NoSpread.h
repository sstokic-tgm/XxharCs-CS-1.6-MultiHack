/* Zippahs Spread */
/* GANS v5.0 */
#ifndef NOSPREAD_H
#define NOSPREAD_H
#include <Windows.h>
#include "gamehooking.h"

class cNoSpread
{
	public:
		void GetSpreadOffset(unsigned int seed, int future, float *inangles, float *velocity, float *outangles);
		void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);

	private:
		unsigned int U_Random( void );
		float GetVecSpread(float speed);
		void DefaultSpreadVar(int weaponid);
		void PrimaryAttack(void);
		void VectorAngles(const float *forward, float *angles);
		void U_Srand( unsigned int seed );
		void GetSpreadXY(unsigned int seed, int future, float *velocity, float *vec);
		int UTIL_SharedRandomLong( unsigned int seed, int low, int high );
		float UTIL_SharedRandomFloat( unsigned int seed, float low, float high );
};
extern cNoSpread gNoSpread;
#endif