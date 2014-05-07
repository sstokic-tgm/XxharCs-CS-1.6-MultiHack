#include <windows.h>
#include "..\console.h"

#define MAX_SPRITES 500

static char *sprites[MAX_SPRITES];


void add_sprite(int id, const char *sprite)
{
	if (id>=0 && id<MAX_SPRITES)
	{
    sprites[id]=strdup(sprite);
  }
}


const char *sprite_lookup (int id)
{
	if (id>=0 && id<MAX_SPRITES && sprites[id])
	{
    return sprites[id];
  }
	else
	{
    return "unknown";
  }
}

