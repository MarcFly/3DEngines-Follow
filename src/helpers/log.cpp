#pragma once
#include <src/helpers/Globals.h>

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);
}

extern pcg32_random_t rngptr_a, rngptr_b;
uint64_t PCGRand() {
	return ((uint64_t)(pcg32_random_r(&rngptr_a)) << 32)
		| pcg32_random_r(&rngptr_b + 1);
}