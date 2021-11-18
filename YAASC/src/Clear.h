#pragma once

#if defined _WIN32
	#include <windows.h>
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
	#include <stdio.h>
#endif

void ClearScreen();
