#ifndef __DEBUG__
#define __DEBUG__
#include<stdio.h>
// open debug
#define DEBUG_LEVEL 1

#ifdef DEBUG_LEVEL

	#define DEBUG(format, ...) printf (format, ##__VA_ARGS__)
	#define UART(format, ...) printf (format, ##__VA_ARGS__)
	// #define DEBUGFL(format,...) printf("FILE: "__FILE__", LINE: %d: "format"/n", __LINE__, ##__VA_ARGS__)

#else

	#define DEBUG(s)

#endif
#endif