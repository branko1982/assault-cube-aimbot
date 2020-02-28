#pragma once

#include <windows.h>
#include <stdio.h>
#include <gl\GL.h>


class CFont {
private:
    unsigned int base;
    unsigned char r;
    unsigned char g;
    unsigned char b;

public:
    CFont(int size, unsigned char r, unsigned char g, unsigned char b) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->base = ::glGenLists(96);

        HDC hdc = ::wglGetCurrentDC();
        HFONT hFont = ::CreateFontA(-size, NULL, NULL, NULL, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Consolas");
        HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);

        ::wglUseFontBitmapsA(hdc, 32, 96, this->base);
        ::SelectObject(hdc, hOldFont);
        ::DeleteObject(hFont);
    }

    void Print(float x, float y, const char* format, ...) {
        ::glColor3ub(this->r, this->g, this->b);
        ::glRasterPos2f(x, y);

        char text[100];
        ::va_list args;

        va_start(args, format);
        ::vsprintf_s(text, 100, format, args);
        va_end(args);

        ::glPushAttrib(GL_LIST_BIT);
        ::glListBase(this->base - 32);
        ::glCallLists(::strlen(text), GL_UNSIGNED_BYTE, text);
        ::glPopAttrib();
    }
};