
#include "moho.h"

extern "C" 
{
	void Logf(const char* fmt, ...);

	extern CWldSession* g_CWldSession;
};

extern "C" void print_hello_world()
{	
	Logf("Hello world!");
}

extern "C" bool ext_ValidateFocusArmyRequest(int army)
{
	UserArmy* userArmy = ((UserArmy*)g_CWldSession->armies.objects_start)+army;

	return true;
}