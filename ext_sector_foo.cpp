
#include "moho.h"

extern "C" 
{
	void Logf(const char* fmt, ...);

	int stricmp(const char* a, const char* b);

	void lua_pushbool(lua_state *L, int b);

	extern CWldSession* g_CWldSession;
	extern Sim* 		g_Sim;
};

extern "C" void print_hello_world()
{	
	Logf("Hello world!");
}

extern "C" bool ext_ValidateFocusArmyRequest(int army)
{
	//UserArmy* userArmy = ((UserArmy**)g_CWldSession->armies.objects_start)[army];

	return true;
}

extern "C" int cxx_AddCommandSourceId(lua_state* lua, const char* playerName, int sourceId)
{
	Sim* sim = g_Sim;
	for(int i=0; i < sim->armies.size(); i++)
	{
		SimArmy* army = (SimArmy*)sim->armies[i];
		if(stricmp(army->nickname.data(), playerName) == 0)
		{
			army->mValidCommandSources.add(sourceId);
			lua_pushbool(lua, true);
			return 1;
		}
	}
	lua_pushbool(lua, false);
	return 1;
}