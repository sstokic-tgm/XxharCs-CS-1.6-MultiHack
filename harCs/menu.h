#ifndef __MENU_H__
#define __MENU_H__

typedef struct {
   int count;
   int maxcount;
   bool active;
}menu_s;

typedef struct {
   bool menu;
   bool chair;
   bool esp;
   bool info;
   bool enable;
}draw_s;

GLuint base;
HDC hDC;
bool FirstInit = false;
int viewportcount=0;
int Last_Key=0;
static GLint	vp[4];
menu_s	menu;
draw_s	draw;

#endif