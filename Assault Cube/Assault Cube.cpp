#include <windows.h>
#include <iostream>


#include "Logger.hpp"
#include "cheat.h"
#include "glDraw.h"

#include <gl\gl.h>
#include "gltext.h"
#include "glDraw.h"
#include "mem.h"
//https://guidedhacking.com/

#include <iostream>
GL::Font glFont;
#define MENU_FONT_HEIGHT 15

GL::twglSwapBuffers owglSwapBuffers; //hooknutá funkcia

BOOL __stdcall hwglSwapBuffers(HDC hDc)
{
	HDC currentHDC = wglGetCurrentDC();

	if (!glFont.bBuilt || currentHDC != glFont.hdc)
	{
		glFont.Build(MENU_FONT_HEIGHT);
	}

	GL::SetupOrtho();
	//Draw here

	cheat_main();
	GL::RestoreGL();

	return owglSwapBuffers(hDc);
}



BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		DisableThreadLibraryCalls(hModule);

		AllocConsole();

		FILE* A;
		FILE* B;
		FILE* C;

		freopen_s(&A, "CONIN$", "r", stdin);
		freopen_s(&B, "CONOUT$", "w", stdout);
		freopen_s(&C, "CONOUT$", "w", stderr);

		GL::Hook((char*)"wglSwapBuffers", (uintptr_t&)owglSwapBuffers, &hwglSwapBuffers);

		DisableThreadLibraryCalls(hModule);

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
