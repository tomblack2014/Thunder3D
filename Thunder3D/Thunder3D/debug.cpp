#include "common.h"
#include <cstdio>
#include <Windows.h>

void LogError(const char* str, const char* file, int line){
	static char buf[256];
	sprintf(buf, "%s(%d): %s\n", file, line, str);
	OutputDebugStringA(buf);
}