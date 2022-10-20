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

static size_t base_buf_size = 128;

uint16_t FitString(char*& buf, const char* format, ...) {
	if (buf != nullptr) delete buf;

	size_t bufsize = base_buf_size;
	char* args = new char[bufsize]; 
	
	static va_list  ap_args;
	size_t size = bufsize;
	size_t retval = 0;
	while (retval < 1) {
		bufsize *= 2;
		delete[] args;
		args = new char[bufsize];
		va_start(ap_args, format);
		retval = vsprintf_s(args, bufsize, format, ap_args);
		va_end(ap_args);
	}
	buf = new char[retval+1];
	sprintf_s(buf, retval+1, "%s", args);

	delete args;	

	return retval+1;
}

extern pcg32_random_t rngptr_a, rngptr_b;
uint64_t PCGRand() {
	return ((uint64_t)(pcg32_random_r(&rngptr_a)) << 32)
		| pcg32_random_r(&rngptr_b + 1);
}

const char* FileName(const char* path) {
	const char* parent_path_fw = strrchr(path, '/');
	const char* parent_path = (parent_path_fw != nullptr) ? parent_path_fw : strrchr(path, '\\');
	return (parent_path  != nullptr) ? parent_path+1 : path; // If // or \ not found, already a filename
}