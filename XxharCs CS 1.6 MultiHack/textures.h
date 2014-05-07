#ifndef TEXTURE_H
#define TEXTURE_H

void InitTextureTypes(struct playermove_s *ppmove);
char GetTextureType(pmtrace_t *tr, float *vecSrc, float *vecEnd);

#endif