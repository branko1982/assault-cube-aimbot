#include "glDraw.h"
//https://guidedhacking.com/

void GL::Hook(char* function, uintptr_t& oFunction, void* hFunction)
{
	HMODULE hMod = GetModuleHandle("opengl32.dll"); //we get the address of the .dll in memory

	if (hMod) //if address is not null
	{
		oFunction = (uintptr_t)mem::TrampolineHook((void*)GetProcAddress(hMod, function), hFunction, 5);
	}
}

void GL::SetupOrtho()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void GL::RestoreGL()
{
	glEnable(GL_DEPTH_TEST);
}

void GL::DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3])
{
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
}

void GL::DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP);
	glColor3ub(color[0], color[1], color[2]);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glVertex2f(x, y);
	glEnd();
}


void DrawRect(int x, int y, int x2, int y2,  GLubyte color[3], bool bFilled)
{
	glDepthMask(GL_FALSE);
	glDisable(GL_TEXTURE_2D);
	glColor3ub(color[0], color[1], color[2]);

	glBegin(bFilled ? GL_QUADS : GL_LINE_LOOP);

	glVertex2f(x, y);
	glVertex2f(x2, y);
	glVertex2f(x2, y2);
	glVertex2f(x, y2);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
}