#include <Windows.h>
#include "NoRecoil.h"
#include "gamehooking.h"

void ApplyNoRecoil(float frametime, float *punchangle, float *viewangle)
{
	float punch[3], length;
	VectorCopy(punchangle, punch);
	length = VectorLength(punch);
	length -= (10.0 + length * 0.5) * frametime;
	length = max(length, 0.0);
	VectorScale(punch, length, punch);
	viewangle[0] += punch[0] + punch[0];
	viewangle[1] += punch[1] + punch[1];
}