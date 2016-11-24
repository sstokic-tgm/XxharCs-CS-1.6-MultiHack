#include <Windows.h>
#include <iostream>
#include "GL\GL.h"
#include "GL\GLU.h"
#include "GL\glaux.h"
#include "GL\glext.h"

#include "menu.h"
#include "cvars.h"
#include "detours.h"

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")

//////////////////////////////////////////////////////////////////////////
extern "C" void APIENTRY wglSwapBuffers(HDC dc);
extern "C" GLint __stdcall ogluProject(GLdouble objX, GLdouble objY, GLdouble objZ, 
                    const GLdouble *model, const GLdouble *proj, const GLint *view, 
                         GLdouble* winX, GLdouble* winY, GLdouble* winZ);

DETOUR_TRAMPOLINE(void APIENTRY glBegin_detour(GLenum mode), glBegin);
DETOUR_TRAMPOLINE(void APIENTRY glEnd_detour(), glEnd);
DETOUR_TRAMPOLINE(void APIENTRY glClear_detour(GLbitfield mask), glClear);
DETOUR_TRAMPOLINE(void APIENTRY glVertex3fv_detour(const GLfloat *v), glVertex3fv);
DETOUR_TRAMPOLINE(void APIENTRY glVertex3f_detour(GLfloat x, GLfloat y, GLfloat z), glVertex3f);
DETOUR_TRAMPOLINE(void APIENTRY glEnable_detour(GLenum cap), glEnable);
DETOUR_TRAMPOLINE(void APIENTRY glVertex2f_detour(GLfloat x, GLfloat y), glVertex2f);
DETOUR_TRAMPOLINE(void APIENTRY glViewport_detour(GLint x, GLint y, GLsizei width, GLsizei height), glViewport);
DETOUR_TRAMPOLINE(void APIENTRY wglSwapBuffers_detour(HDC dc), wglSwapBuffers);
DETOUR_TRAMPOLINE(void APIENTRY glBlendFunc_detour(GLenum sfactor, GLenum dfactor), glBlendFunc);
DETOUR_TRAMPOLINE(void APIENTRY glPopMatrix_detour(void), glPopMatrix);
DETOUR_TRAMPOLINE(void APIENTRY glTranslatef_detour(GLfloat x, GLfloat y, GLfloat z), glTranslatef);
DETOUR_TRAMPOLINE(void APIENTRY glPushMatrix_detour(void), glPushMatrix);

typedef struct _ENTITIES{
	FLOAT Origin[3];
	BYTE Visible;
	BYTE Valid;
	POINT ESP;
	POINT Head;
	FLOAT BoxHeight;
}ENTITIES,*PENTITIES;

ENTITIES ents[33] = { NULL };

float fCamera[3];
float fLastPosition[3];
float fHighestVertex[3];
float fLowestVertex[3];
float fCenterVertex[3];

int viewport[4];
int matrixDepth = 0;
int vertexCount3f = 0;
int maxEnts = 0;
double the_Double = 0.5000000000000000;

bool found_An_Entity = FALSE;

bool oglSubtractive = false;
bool bDrawingScope = false;
bool bDrawnWorld = false;
bool bStartedDrawingEnts = false;
bool bSkyTex = false;
bool bDrawingFlash = false;
bool bDrawingSmoke = false;

bool bSmoke = false;
bool bFlash = false;
bool bCrosshairDrawn = false;
bool bSky = false;
GLfloat coords[4];
//////////////////////////////////////////////////////////////////////////

float WINAPI GetDistance(float * LocationA, float * LocationB)
{
	float DistanceX		= LocationA[0] - LocationB[0];
	float DistanceY		= LocationA[1] - LocationB[1];
	float DistanceZ		= LocationA[2] - LocationB[2];

	return (float)sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY) + (DistanceZ * DistanceZ));
}

void InitMenu() {
   menu.count = 0; 
   menu.active = false;
   draw.menu = false;
}

void MenuUp()
{
   if(menu.count < 1)
		menu.count = menu.maxcount-1;
   else
		menu.count--;
}

void MenuDown()
{
    if(menu.count > menu.maxcount-2)
		menu.count = 0;
    else
		menu.count++;
}

void MenuSelect()
{
	switch(menu.count)
	{
	case 0:

		cvars.dmAimbot =! cvars.dmAimbot;
		break;

	case 1:

		cvars.aimthrough =! cvars.aimthrough;
		break;

	case 2:
    
		cvars.wallhack =! cvars.wallhack;
		break;
      
	case 3:
	
		cvars.smokeRemoval =! cvars.smokeRemoval;
		break;
	
	case 4:
	
		cvars.flashRemoval =! cvars.flashRemoval;
		break;
	
	case 5:
	
	    cvars.skyRemoval =! cvars.skyRemoval;
		break;
	
	case 6:
	
	    cvars.lambert =! cvars.lambert;
		break;
	
	case 7:
	
	    cvars.whiteWalls =! cvars.whiteWalls;
		break;
	
	case 8:
	
	    cvars.transparentWalls =! cvars.transparentWalls;
		break;
	
	case 9:
	
	    cvars.wireframe =! cvars.wireframe;
		if(cvars.wireframe)
			cvars.skyRemoval = true;
		else
			cvars.skyRemoval = false;

		break;
	
	case 10:
	
		cvars.wireframeModels =! cvars.wireframeModels;
		break;

	case 11:

		cvars.hud =! cvars.hud;
		break;
	}
}

void glPrint(float x, float y, float z, float r, float g, float b, const char *fmt, ...)
{
	char text[256];						
	va_list	ap;

	if(fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	
	GLfloat  curcolor[4], position[4];
	(*glPushAttrib)(GL_ALL_ATTRIB_BITS);
	(*glGetFloatv)(GL_CURRENT_COLOR, curcolor);
	(*glGetFloatv)(GL_CURRENT_RASTER_POSITION, position);
	
	(*glDisable)(GL_TEXTURE_2D); 
	(*glColor4f)(r, g, b, 1.0f);
	(*glRasterPos3f)(x, y, z);
	(*glPushAttrib)(GL_LIST_BIT);
	(*glListBase)(base - 32);
	(*glCallLists)(strlen(text), GL_UNSIGNED_BYTE, text);
	(*glPopAttrib)();
	(*glEnable)(GL_TEXTURE_2D);
	
	(*glPopAttrib)();
	(*glColor4fv)(curcolor);
	(*glRasterPos2f)(position[0], position[1]);
}

GLvoid BuildFont(GLvoid)
{
	hDC=wglGetCurrentDC();
	HFONT	font;
	HFONT	oldfont;
	base = (*glGenLists)(96);
	font = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE|DEFAULT_PITCH, "Tahoma");
	oldfont = (HFONT)SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, base);
	SelectObject(hDC, oldfont);
	DeleteObject(font);									
}

void DrawMenu(int x, int y)
{
	char Entry[12][30];
	int c=0;

	sprintf(Entry[c],"DM-Aimbot  %s", cvars.dmAimbot ? "On":"Off"); c++;
	sprintf(Entry[c],"Aimthrough  %s", cvars.aimthrough ? "On":"Off"); c++;
	sprintf(Entry[c],"Wallhack  %s", cvars.wallhack ? "On":"Off"); c++;
	sprintf(Entry[c],"Smoke Removal  %s", cvars.smokeRemoval ? "On":"Off"); c++;
	sprintf(Entry[c],"Flashbang Removal  %s", cvars.flashRemoval ? "On":"Off"); c++;
	sprintf(Entry[c],"Sky Removal  %s", cvars.skyRemoval ? "On":"Off"); c++;
	sprintf(Entry[c],"Lambert  %s", cvars.lambert ? "On":"Off"); c++;
	sprintf(Entry[c],"White Walls  %s", cvars.whiteWalls ? "On":"Off"); c++;
	sprintf(Entry[c],"Transparent Walls  %s", cvars.transparentWalls ? "On":"Off"); c++;
	sprintf(Entry[c],"Wireframe  %s", cvars.wireframe ? "On":"Off"); c++;
	sprintf(Entry[c],"Wireframe Models  %s", cvars.wireframeModels ? "On":"Off"); c++;
	sprintf(Entry[c],"New HUD  %s", cvars.hud ? "On":"Off"); c++;

	menu.maxcount = c;
	
	
	glPrint(x, y-28, 1.0f, 0.5f, 0.5f, 1.0f,"-------------------------------------");
	glPrint(x, y-17, 1.0f, 0.5f, 0.5f, 1.0f,"-       XxharCs Multihack        -");
	glPrint(x, y-6, 1.0f, 0.5f, 0.5f, 1.0f,"-------------------------------------");
	
   
	for (int i = 0; i < menu.maxcount; i++)
	{
		if (menu.count == i) 
			glPrint(x, y+(14*i)+5, 1.0f, 1.0f, 0.2f, 0.2f, Entry[i]); 
		else
			glPrint(x, y+(14*i)+5, 1.0f, 1.0f, 1.0f, 1.0f, Entry[i]);
	}
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glBegin(GLenum mode)
{

	// entity draw start
	if(!bStartedDrawingEnts && bDrawnWorld && cvars.wallhack > 1){
		if(mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN){
			bStartedDrawingEnts = true;
			(*glClear_detour)(GL_DEPTH_BUFFER_BIT);
		}
	}

	if(cvars.dmAimbot && (mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN) && found_An_Entity == FALSE)
	{
		found_An_Entity = TRUE;
	}
			
	if(cvars.dmAimbot && (mode == GL_POLYGON || mode == GL_TRIANGLES || mode == GL_QUADS || mode == GL_QUAD_STRIP) && found_An_Entity == TRUE)
	{
		found_An_Entity = FALSE;
	}


	if (cvars.wallhack)
	{
		if(mode == GL_TRIANGLES || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN)
		{
			glDepthRange(0, 0.5);
		}
		else
		{
			glDepthRange(0.5, 1);
		}
	}

	if(cvars.smokeRemoval)
	{
		if(mode == GL_QUADS)
		{
			GLfloat smokecol[4];
			(*glGetFloatv)(GL_CURRENT_COLOR, smokecol);

			if((smokecol[0] == smokecol[1]) && (smokecol[0] == smokecol[2]) && (smokecol[0] != 0.0) && (smokecol[0] != 1.0)) 
			{
				bSmoke=true;
			}
			else
			{
				bSmoke=false;
			}
		}
	}

	if(cvars.flashRemoval)
	{
		if(mode == GL_QUADS)
		{
			GLfloat flashcol[4];
			(*glGetFloatv)(GL_CURRENT_COLOR, flashcol);

			if((flashcol[0] == 1.0) && (flashcol[1] == 1.0) && (flashcol[2] == 1.0))
			{
				(*glGetFloatv)(GL_VIEWPORT, coords);
				bFlash=true;
			}
			else
			{
				bFlash=false;
			}
		}
	}

	if(cvars.skyRemoval)
	{
		if(mode == GL_QUADS)
		{
			bSky = true;
		}
		else
		{
			bSky = false;
		}
	}

	if(cvars.transparentWalls)
	{
		if(mode == GL_TRIANGLES || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN || mode == GL_QUADS)
		{
			GLfloat curcol[4];
			(*glGetFloatv)(GL_CURRENT_COLOR, curcol);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);

			(*glBlendFunc_detour)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glColor4f(curcol[0], curcol[1], curcol[2], 0.5f);
		}
	}

	if(cvars.whiteWalls)
	{
		if(mode == GL_POLYGON)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		}
	}

	if(cvars.wireframe)
	{
		if(mode == GL_POLYGON)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(1.0) ;
			glColor3f(0.0f, 0.7f, 0.0f);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if(cvars.wireframeModels)
	{
		if(mode==GL_TRIANGLE_STRIP || mode==GL_TRIANGLE_FAN)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
			glLineWidth(1.0);  
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}
	}

	if(glBegin_detour)
		(*glBegin_detour)(mode);


}


//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glClear(GLbitfield mask)
{
	if(mask == GL_DEPTH_BUFFER_BIT)
	{
		(*glClear_detour)(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	(*glClear_detour)(mask);
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glVertex3fv(const GLfloat *v)
{
	if(bSmoke == true)
		return;

	if(bSky)
		return;

	(*glVertex3fv_detour)(v);
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY xwglSwapBuffers(HDC hDC)
{
	bDrawnWorld = false;
	bStartedDrawingEnts = false;
	viewportcount = 0;

	if(cvars.dmAimbot)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0, viewport[2], viewport[3], 0, 0, 1);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);
		glTranslatef(0.375f, 0.375f, 0.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for(int i = 0; i < maxEnts; i++)
		{
			float x = (float)ents[i].ESP.x;
			float y = (float)ents[i].ESP.y;
			float w = 1;
			float h = ents[i].BoxHeight;
			UCHAR r, g, b, a;
			
			
			if(ents[i].Visible == 0x1)
			{	
				r = 20;
				g = 255;
				b = 20;
				a = 200;
			}
			else
			{
				r = 255;
				g = 255;
				b = 20;
				a = 200;
			}

			glColor4ub(r, g, b, a);
			glBegin(GL_QUADS);

			glVertex2f(x - h - w, y - h - w);
			glVertex2f(x + h + w, y - h - w);
			glVertex2f(x + h + w, y - h);
			glVertex2f(x - h - w, y - h);
		
			glVertex2f(x - h - w, y + h - w);
			glVertex2f(x + h + w, y + h - w);
			glVertex2f(x + h + w, y + h);
			glVertex2f(x - h - w, y + h);

			glVertex2f(x - h - w, y - h);
			glVertex2f(x - h, y - h);
			glVertex2f(x - h, y + h);
			glVertex2f(x - h - w, y + h);

			glVertex2f(x + h, y - h);
			glVertex2f(x + h + w, y - h);
			glVertex2f(x + h + w, y + h);
			glVertex2f(x + h, y + h);

			glEnd();
		}

		float fClosestPos = 999999.0f;
		DWORD TargetX = 0;
		DWORD TargetY = 0;

		for(int i = 0; i < maxEnts; i++)
		{
			float fTemp[3];
			float ScrCenter[3];

			ScrCenter[0] = float(viewport[2] / 2); 
			ScrCenter[1] = float(viewport[3] / 2);
			ScrCenter[2] = 0.0f;

			fTemp[0] = (float)ents[i].Head.x;
			fTemp[1] = (float)ents[i].Head.y;
			fTemp[2] = 0.0f;

			float nDist = GetDistance(fTemp, ScrCenter);

			if((cvars.aimthrough || (!cvars.aimthrough && ents[i].Visible == 0x1)))
			{
				if(nDist < fClosestPos)
				{
					fClosestPos = nDist;
					TargetX = ents[i].Head.x;
					TargetY = ents[i].Head.y;
				}
			}
		}

		if((GetAsyncKeyState(VK_LBUTTON) < 0))
		{

			if(TargetX && TargetY)
			{
				POINT Target;
				DWORD ScrX = viewport[2] / 2; 
				DWORD ScrY = viewport[3] / 2;

				if(TargetX > (ScrX))
				{
					Target.x = TargetX - (ScrX);
					Target.x /= 4;
					Target.x = +Target.x;
				}

				if(TargetX < (ScrX))
				{
					Target.x = (ScrX) - TargetX;
					Target.x /= 4;
					Target.x = -Target.x;
				}

				if(TargetX == (ScrX))
				{
					Target.x = 0;
				}

				if(TargetY > (ScrY))
				{
					Target.y = TargetY - (ScrY);
					Target.y /= 4;
					Target.y = +Target.y;
				}

				if(TargetY < (ScrY))
				{
					Target.y = (ScrY) - TargetY;
					Target.y /= 4;
					Target.y = -Target.y;
				}

				if(TargetY == (ScrY))
				{
					Target.y = 0;
				}

				mouse_event(MOUSEEVENTF_MOVE, Target.x, Target.y, NULL, NULL);
			}
		}

		glDisable(GL_BLEND);
		
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);

		maxEnts = 0;
	}
	

	(*wglSwapBuffers_detour)(hDC);
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glEnd()
{
	(*glEnd_detour)();
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glVertex2f(GLfloat x, GLfloat y)
{
	if(bFlash == true)
	{
		if(y == coords[3])
		{
			GLfloat fcurcol[4];
			glGetFloatv(GL_CURRENT_COLOR, fcurcol);
			glColor4f(fcurcol[0], fcurcol[1], fcurcol[2], 0.01f);
		}
	}

	(*glVertex2f_detour)(x, y);
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	if(cvars.lambert)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	
	(*glVertex3f_detour)(x, y, z);

	if(cvars.dmAimbot)
	{
		vertexCount3f++;

		fLastPosition[0] = x;
		fLastPosition[1] = y;
		fLastPosition[2] = z;

		fCenterVertex[0] += x;
		fCenterVertex[1] += y;
		fCenterVertex[2] += z;

		if(z > fHighestVertex[2] || vertexCount3f == 1)
		{
			fHighestVertex[0] = fLastPosition[0];
			fHighestVertex[1] = fLastPosition[1];
			fHighestVertex[2] = fLastPosition[2];
		}
	
		if(z < fLowestVertex[2] || vertexCount3f == 1)
		{
			fLowestVertex[0] = fLastPosition[0];
			fLowestVertex[1] = fLastPosition[1];
			fLowestVertex[2] = fLastPosition[2];
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glEnable (GLenum cap)
{
	
	if(bCrosshairDrawn == false)
	{
		glPushMatrix();
		glLoadIdentity();
		

		GLint iDim[4];
		(*glGetIntegerv)(GL_VIEWPORT, iDim);
		glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
		glLineWidth(3.0f);

		glBegin(GL_LINES);
		glVertex2i(iDim[2] / 2, (iDim[3] / 2) - 12);
		glVertex2i(iDim[2] / 2, (iDim[3] / 2) - 5);

		glVertex2i(iDim[2] / 2, (iDim[3] / 2) + 5);
		glVertex2i(iDim[2] / 2, (iDim[3] / 2) + 12);

		glVertex2i((iDim[2] / 2) - 12, iDim[3] / 2);
		glVertex2i((iDim[2] / 2) - 5, iDim[3] / 2);

		glVertex2i((iDim[2] / 2) + 5, iDim[3] / 2);
		glVertex2i((iDim[2] / 2) + 12, iDim[3]/2);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);

		glBegin(GL_POINTS);
		glVertex2i((iDim[2] / 2) - 3, iDim[3] / 2);
		glVertex2i((iDim[2] / 2) + 3, iDim[3] / 2);
		glVertex2i(iDim[2] / 2, (iDim[3] / 2) - 3);
		glVertex2i(iDim[2] / 2, (iDim[3] / 2) + 3);
		glEnd();

		
		glPopMatrix();
		bCrosshairDrawn = true;
	}

	if(!FirstInit) 
	{
		BuildFont();
		(*glGetIntegerv)(GL_VIEWPORT, vp);
		FirstInit = true;
		InitMenu();
	}
	
	if(draw.enable)
	{
		draw.enable = false;

		if (draw.menu)
			DrawMenu(50, (vp[3]/2)-60);
	}
	
	(*glEnable_detour)(cap);
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	bCrosshairDrawn = false;

	if(cvars.dmAimbot)
	{
		viewport[0] = x;
		viewport[1] = y;
		viewport[2] = width;
		viewport[3] = height;
	}

	viewportcount++;
	if (viewportcount > 4)
		draw.enable = true;

	if(draw.enable)
	{
		if((GetAsyncKeyState(VK_NUMPAD8) < 0) && (Last_Key != VK_NUMPAD8) && (menu.active))
		{
			MenuUp();
			Last_Key = VK_NUMPAD8;
		}
		else if((GetAsyncKeyState(0x50) < 0) && (Last_Key != 0x50))
		{
			menu.active = !menu.active;
			draw.menu = !draw.menu;
			Last_Key = 0x50;
		}
		else if((GetAsyncKeyState(VK_NUMPAD4) < 0) && (Last_Key != VK_NUMPAD4) && (menu.active))
		{
			MenuSelect();
			Last_Key = VK_NUMPAD4;
		}
		else if((GetAsyncKeyState(VK_NUMPAD6) < 0) && (Last_Key != VK_NUMPAD4) && (menu.active))
		{
			MenuSelect();
			Last_Key = VK_NUMPAD4;
		}
		else if((GetAsyncKeyState(VK_NUMPAD2) < 0) && (Last_Key != VK_NUMPAD4) && (menu.active))
		{
			MenuDown();
			Last_Key = VK_NUMPAD4;
		}
		else if(!(GetAsyncKeyState(VK_NUMPAD2) < 0) && 
				!(GetAsyncKeyState(VK_NUMPAD6) < 0) && 
				!(GetAsyncKeyState(0x50) < 0) && 
				!(GetAsyncKeyState(VK_NUMPAD8) < 0) && 
				!(GetAsyncKeyState(VK_NUMPAD4) < 0))
					Last_Key = 0;
	}

	(*glViewport_detour)(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////
void APIENTRY hooked_glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	if(cvars.hud && dfactor == GL_ONE)
	{
		glColor3f(0.3f, 0.3f, 1.0f);
	}

	(*glBlendFunc_detour)(sfactor, dfactor);
}

void APIENTRY hooked_glPopMatrix(void)
{
	(*glPopMatrix_detour)();

	if(cvars.dmAimbot)
	{
		matrixDepth--;
	
		if(vertexCount3f > 550 && GetDistance(fHighestVertex, fCamera)  > 50.0f
			&& (fHighestVertex[2] - fLowestVertex[2]) > 30.0f)
		{
			double ModelView[16];
			double ProjView[16];
			double View2D[3];
			double Depthcheck[3];
			GLfloat x = 0.0f, y = 0.0f, pix;
			FLOAT fVertexH[3];
			FLOAT fVertexL[3];
			FLOAT BoxH[2];
			FLOAT BoxL[2];
			FLOAT Head[2];

			fCenterVertex[0] /= vertexCount3f;
			fCenterVertex[1] /= vertexCount3f;
			fCenterVertex[2] /= vertexCount3f;

			fVertexH[0] = fHighestVertex[0];
			fVertexH[1] = fHighestVertex[1];
			fVertexH[2] = fHighestVertex[2];
			fVertexH[2] += 10.0f;

			fVertexL[0] = fHighestVertex[0];
			fVertexL[1] = fHighestVertex[1];
			fVertexL[2] = fHighestVertex[2];
			fVertexL[2] -= 10.0f;

			ents[maxEnts].Visible = 0x0;

			glGetDoublev(GL_MODELVIEW_MATRIX, ModelView);
			glGetDoublev(GL_PROJECTION_MATRIX, ProjView) ;

			if(ogluProject(fHighestVertex[0], fHighestVertex[1], fHighestVertex[2] - 4, ModelView, ProjView,
				viewport, &View2D[0], &View2D[1], &View2D[2]) == GL_TRUE)
			{
				Head[0] = (float)View2D[0];
				Head[1] = (int)viewport[3]-(float)View2D[1];

				if(ogluProject(fVertexH[0], fVertexH[1], fVertexH[2], ModelView, ProjView,
					viewport, &View2D[0], &View2D[1], &View2D[2]) == GL_TRUE)
				{
					BoxH[0] = (float)View2D[0];
					BoxH[1] = (int)viewport[3]-(float)View2D[1];

					if(ogluProject(fVertexL[0], fVertexL[1], fVertexL[2], ModelView, ProjView,
						viewport, &View2D[0], &View2D[1], &View2D[2]) == GL_TRUE)
					{
						BoxL[0] = (float)View2D[0];
						BoxL[1] = (int)viewport[3]-(float)View2D[1];

						if(ogluProject(fCenterVertex[0], fCenterVertex[1], fCenterVertex[2], ModelView, ProjView,
							viewport, &View2D[0], &View2D[1], &View2D[2]) == GL_TRUE)
						{
							x = (float)View2D[0];
							y = (int)viewport[3]-(float)View2D[1];

							if(ogluProject(fHighestVertex[0], fHighestVertex[1], fHighestVertex[2] + 1, ModelView, ProjView,
								viewport, &Depthcheck[0], &Depthcheck[1], &Depthcheck[2]) == GL_TRUE)
							{
								glReadPixels((int)Depthcheck[0], (int)Depthcheck[1], 1, 1, 
									GL_DEPTH_COMPONENT, GL_FLOAT, &pix);

								if(pix >= Depthcheck[2])
									ents[maxEnts].Visible = 0x1;

								ents[maxEnts].Origin[0] = fCenterVertex[0];
								ents[maxEnts].Origin[1] = fCenterVertex[1];
								ents[maxEnts].Origin[2] = fCenterVertex[2];
								
								ents[maxEnts].ESP.x = (LONG)x;
								ents[maxEnts].ESP.y = (LONG)y;

								ents[maxEnts].Head.x = (LONG)Head[0];
								ents[maxEnts].Head.y = (LONG)Head[1];

								float BoxHeight = BoxL[1] - BoxH[1];

								BoxHeight = BoxHeight / 2;

								if(BoxHeight < 7)
									ents[maxEnts].BoxHeight = 6;
								else
									ents[maxEnts].BoxHeight = BoxHeight;

								maxEnts++;
							}
						}
					}
				}
			}
		}
		fCenterVertex[0] = 0.0f;
		fCenterVertex[1] = 0.0f;
		fCenterVertex[2] = 0.0f;
	}
}

void APIENTRY hooked_glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	if(cvars.dmAimbot)
	{
		if(x != 0.0f && y != 0.0f && z != 0.0f)
		{
			fCamera[0] = -x;
			fCamera[1] = -y;
			fCamera[2] = -z;
		}
	}

	(*glTranslatef_detour)(x, y, z);
}

void APIENTRY hooked_glPushMatrix(void)
{
	(*glPushMatrix_detour)();

	if(cvars.dmAimbot)
	{
		matrixDepth++;
		vertexCount3f = 0;
	}
}

extern "C" __declspec(naked)
GLint __stdcall ogluProject(GLdouble objX, GLdouble objY, GLdouble objZ, 
                    const GLdouble *model, const GLdouble *proj, const GLint *view, 
                         GLdouble* winX, GLdouble* winY, GLdouble* winZ)
{
    __asm
    {
        MOV EDI,EDI
        PUSH EBP
        MOV EBP,ESP
        PUSH ECX
        SUB ESP,0x40
        FLD QWORD PTR SS:[EBP+0x8]
        LEA EAX,DWORD PTR SS:[EBP-0x40]
        FSTP QWORD PTR SS:[EBP-0x20]
        PUSH EAX                               
        FLD QWORD PTR SS:[EBP+0x10]              
        LEA EAX,DWORD PTR SS:[EBP-0x20]           
        FSTP QWORD PTR SS:[EBP-0x18]           
        PUSH EAX                                
        FLD QWORD PTR SS:[EBP+0x18]              
        PUSH DWORD PTR SS:[EBP+0x20]         
        FSTP QWORD PTR SS:[EBP-0x10]         
        FLD1                                   
        FSTP QWORD PTR SS:[EBP-0x8]                
        CALL PartB                                
        LEA EAX,DWORD PTR SS:[EBP-0x20]
        PUSH EAX                                
        LEA EAX,DWORD PTR SS:[EBP-0x40]            
        PUSH EAX                                
        PUSH DWORD PTR SS:[EBP+0x24]            
        CALL PartB                                
        FLDZ
        FLD QWORD PTR SS:[EBP-0x8]
        FUCOM ST(0x1)
        FSTSW AX
        FSTP ST(0x1)
        TEST AH,0x44
        JPE jmp1
        FSTP ST
        XOR EAX,EAX
        JMP jmp2
        jmp1: FLD QWORD PTR SS:[EBP-0x20]
        MOV EAX,DWORD PTR SS:[EBP+0x28]
        FDIV ST,ST(0x1)
        FLD QWORD PTR SS:[EBP-0x18]
        FDIV ST,ST(0x2)
        FLD QWORD PTR SS:[EBP-0x10]
        FDIVRP ST(0x3),ST
        FLD QWORD PTR DS:[the_Double]
        FMUL ST(0x2),ST
        FADD ST(0x2),ST
        FMUL ST(0x1),ST
        FADD ST(0x1),ST
        FMUL ST(0x3),ST
        FADDP ST(0x3),ST
        FILD DWORD PTR DS:[EAX+0x8]
        FMULP ST(0x2),ST
        FILD DWORD PTR DS:[EAX]
        FADDP ST(0x2),ST
        FIMUL DWORD PTR DS:[EAX+0xC]
        FIADD DWORD PTR DS:[EAX+0x4]
        MOV EAX,DWORD PTR SS:[EBP+0x2C]
        FXCH ST(0x1)
        FSTP QWORD PTR DS:[EAX]
        MOV EAX,DWORD PTR SS:[EBP+0x30]
        FSTP QWORD PTR DS:[EAX]
        MOV EAX,DWORD PTR SS:[EBP+0x34]
        FSTP QWORD PTR DS:[EAX]
        XOR EAX,EAX
        INC EAX
        jmp2: LEAVE
        RETN 0x30

PartB:
        MOV EDI,EDI
        PUSH EBP
        MOV EBP,ESP
        MOV EAX,DWORD PTR SS:[EBP+0x8]
        MOV ECX,DWORD PTR SS:[EBP+0xC]
        XOR EDX,EDX
        ADD EAX,0x40
        PUSH ESI
        jmpa: FLD QWORD PTR DS:[EAX-0x40]
        MOV ESI,DWORD PTR SS:[EBP+0x10]
        FMUL QWORD PTR DS:[ECX]
        FLD QWORD PTR DS:[EAX-0x20]
        FMUL QWORD PTR DS:[ECX+0x8]
        FADDP ST(0x1),ST
        FLD QWORD PTR DS:[EAX]
        ADD EAX,0x8
        FMUL QWORD PTR DS:[ECX+0x10]
        FADDP ST(0x1),ST
        FLD QWORD PTR DS:[EAX+0x18]
        FMUL QWORD PTR DS:[ECX+0x18]
        FADDP ST(0x1),ST
        FSTP QWORD PTR DS:[ESI+EDX*0x8]
        INC EDX
        CMP EDX,0x4
        JL jmpa
        POP ESI
        POP EBP
        RETN 0x0C
    }
}


void InstallGL()
{
	DetourFunctionWithTrampoline((PBYTE)glBegin_detour, (PBYTE)hooked_glBegin);
	DetourFunctionWithTrampoline((PBYTE)glEnd_detour, (PBYTE)hooked_glEnd);
	DetourFunctionWithTrampoline((PBYTE)glClear_detour, (PBYTE)hooked_glClear);
	DetourFunctionWithTrampoline((PBYTE)glEnable_detour, (PBYTE)hooked_glEnable);
	DetourFunctionWithTrampoline((PBYTE)glVertex3fv_detour, (PBYTE)hooked_glVertex3fv);
	DetourFunctionWithTrampoline((PBYTE)glVertex3f_detour, (PBYTE)hooked_glVertex3f);
	DetourFunctionWithTrampoline((PBYTE)glVertex2f_detour, (PBYTE)hooked_glVertex2f);
	DetourFunctionWithTrampoline((PBYTE)glViewport_detour, (PBYTE)hooked_glViewport);
	DetourFunctionWithTrampoline((PBYTE)wglSwapBuffers_detour, (PBYTE)xwglSwapBuffers);
	DetourFunctionWithTrampoline((PBYTE)glBlendFunc_detour, (PBYTE)hooked_glBlendFunc);
	DetourFunctionWithTrampoline((PBYTE)glPopMatrix_detour, (PBYTE)hooked_glPopMatrix);
	DetourFunctionWithTrampoline((PBYTE)glPushMatrix_detour, (PBYTE)hooked_glPushMatrix);
}

void UninstallGL()
{
	DetourRemove((PBYTE)hooked_glBegin, (PBYTE)glBegin_detour);
	DetourRemove((PBYTE)hooked_glEnd, (PBYTE)glEnd_detour);
	DetourRemove((PBYTE)hooked_glClear, (PBYTE)glClear_detour);
	DetourRemove((PBYTE)hooked_glEnable, (PBYTE)glEnable_detour);
	DetourRemove((PBYTE)hooked_glVertex3fv, (PBYTE)glVertex3fv_detour);
	DetourRemove((PBYTE)hooked_glVertex3f, (PBYTE)glVertex3f_detour);
	DetourRemove((PBYTE)hooked_glVertex2f, (PBYTE)glVertex2f_detour);
	DetourRemove((PBYTE)hooked_glViewport, (PBYTE)glViewport_detour);
	DetourRemove((PBYTE)xwglSwapBuffers, (PBYTE)wglSwapBuffers_detour);
	DetourRemove((PBYTE)hooked_glBlendFunc, (PBYTE)glBlendFunc_detour);
	DetourRemove((PBYTE)hooked_glPopMatrix, (PBYTE)glPopMatrix_detour);
	DetourRemove((PBYTE)hooked_glPushMatrix, (PBYTE)glPushMatrix_detour);
}