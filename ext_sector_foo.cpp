
#include "moho.h"

extern "C" void Logf(const char* fmt, ...);

extern "C" void print_hello_world()
{	
	Logf("Hello world!");
}