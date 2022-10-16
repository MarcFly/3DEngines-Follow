#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>
#include <src/helpers/pcg/pcg_basic.h>
uint64_t PCGRand();

#define PLOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI


typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "3D Physics Playground"

// XMacros

#define EXPAND_AS_ENUM(a,b) a,
#define EXPAND_AS_VALUE(a,b) b,
// In corner cases need more than 2 values, it is pushing a bit really but it should work
#define X3_EXPAND_1(a,b,c) a,
#define X3_EXPAND_2(a,b,c) b,
#define X3_EXPAND_3(a,b,c) c,

#define XM_EXPAND_1(a,b,c,d) a,
#define XM_EXPAND_2(a,b,c,d) b,
#define XM_EXPAND_3(a,b,c,d) c,
#define XM_EXPAND_4(a,b,c,d) d,