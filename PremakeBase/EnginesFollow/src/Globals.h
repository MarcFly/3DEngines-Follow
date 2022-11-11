#pragma once

#include "EnginePCH.h"

#ifndef EF_API																															
#   ifdef _WIN32																														
#       if defined(EF_BUILD_SHARED) /* build dll */																						
#           define EF_API __declspec(dllexport)																							
#       elif !defined(EF_BUILD_STATIC) /* use dll */
#			define EF_API __declspec(dllimport)
#		else
#			define EF_API
#       endif																															
#   else																																
#       define EF_API																													
#   endif																																
#endif	

typedef void(VoidFun)();
typedef unsigned char byte;

EF_API uint64_t simplehash(const char* str);

namespace Engine {
	enum BitFlags
	{
		BITSET1 = 1 << 0, BITSET17 = 1 << 16,	// BITSET33 = 1 << 32,	BITSET49 = 1 << 48,
		BITSET2 = 1 << 1, BITSET18 = 1 << 17,	// BITSET34 = 1 << 33,	BITSET50 = 1 << 49,
		BITSET3 = 1 << 2, BITSET19 = 1 << 18,	// BITSET35 = 1 << 34,	BITSET51 = 1 << 50,
		BITSET4 = 1 << 3, BITSET20 = 1 << 19,	// BITSET36 = 1 << 35,	BITSET52 = 1 << 51,
		BITSET5 = 1 << 4, BITSET21 = 1 << 20,	// BITSET37 = 1 << 36,	BITSET53 = 1 << 52,
		BITSET6 = 1 << 5, BITSET22 = 1 << 21,	// BITSET38 = 1 << 37,	BITSET54 = 1 << 53,
		BITSET7 = 1 << 6, BITSET23 = 1 << 22,	// BITSET39 = 1 << 38,	BITSET55 = 1 << 54,
		BITSET8 = 1 << 7, BITSET24 = 1 << 23,	// BITSET40 = 1 << 39,	BITSET56 = 1 << 55,
		BITSET9 = 1 << 8, BITSET25 = 1 << 24,	// BITSET41 = 1 << 40,	BITSET57 = 1 << 56,
		BITSET10 = 1 << 9, BITSET26 = 1 << 25,	// BITSET42 = 1 << 41,	BITSET58 = 1 << 57,
		BITSET11 = 1 << 10, BITSET27 = 1 << 26,	// BITSET43 = 1 << 42,	BITSET59 = 1 << 58,
		BITSET12 = 1 << 11, BITSET28 = 1 << 27,	// BITSET44 = 1 << 43,	BITSET60 = 1 << 59,
		BITSET13 = 1 << 12, BITSET29 = 1 << 28,	// BITSET45 = 1 << 44,	BITSET61 = 1 << 60,
		BITSET14 = 1 << 13, BITSET30 = 1 << 29,	// BITSET46 = 1 << 45,	BITSET62 = 1 << 61,
		BITSET15 = 1 << 14, BITSET31 = 1 << 30,	// BITSET47 = 1 << 46,	BITSET63 = 1 << 62,
		BITSET16 = 1 << 15, BITSET32 = 1 << 31	// BITSET48 = 1 << 47,	BITSET64 = 1 << 63

	};

};

#define SET_FLAG(n, f) ((n) |= (f)) 
#define CLEAR_FLAG(n, f) ((n) &= ~(f))
#define APPLY_MASK(n, f) ((n) &= (f))
#define TOGGLE_FLAG(n, f) ((n) ^= (f)) 
#define CHECK_FLAG(n, f) (((n) & (f)) > 0)

#include <stdarg.h>
struct offload_str {
	offload_str(int wanted_size, const char* fmt, ...) : size(wanted_size) { 
		str = new char[wanted_size]; 
		va_list args; 
		va_start(args, fmt);  
		vsnprintf(str, wanted_size, fmt, args); 
		va_end(args); 
	}
	offload_str(offload_str& _str) {
		str = _str.str;
		size = _str.size;

		_str.str = nullptr;
		_str.size = 0;
	}
	~offload_str() { if (str != nullptr) delete str; }
	char* str = nullptr;
	int size = 0;
};

#include <pcg_basic.h>

extern pcg32_random_t rngptr_a, rngptr_b;
static uint64_t PCGRand64() {
	return ((uint64_t)(pcg32_random_r(&rngptr_a)) << 32)
		| pcg32_random_r(&rngptr_b + 1);
}

static uint64_t PCGRand32() {
	return pcg32_random_r(&rngptr_b + 1);
}

const char* FileNameExt(const char* path);
void FileName(const char* path, char* out_name);
std::string ParentPath(const std::string& path);