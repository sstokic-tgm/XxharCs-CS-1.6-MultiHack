#ifndef __EVENTHOOK_H__
#define __EVENTHOOK_H__

void HookEvent(char *name, void (*pfnEvent)(struct event_args_s *args));
void PlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);

#endif