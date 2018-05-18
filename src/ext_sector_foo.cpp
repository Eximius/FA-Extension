
#include "ex_core.h"

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
extern "C" int cxx_SetCommandSourceId(lua_state* lua, int armyId, int sourceId, bool set_or_unset)
{
	Sim* sim = g_Sim;
	SimArmy* army = (SimArmy*)sim->armies[armyId];

	if(set_or_unset)
		army->mValidCommandSources.add(sourceId);
	else
		army->mValidCommandSources.remove(sourceId);
	lua_pushbool(lua, true);
	return 1;
}

#include <cmath>

extern "C" double floor(double x){
	return std::floor(x);
}

extern "C" void *memset(char *str, int c, size_t n)
{
	for(int i=0;i<n;i++)
		str[i] = c;
}

#include <stdarg.h>

extern "C" int vfprintf ( FILE * stream, const char * format, va_list arg );

extern "C" int fprintf(FILE *stream, const char *format, ...)
{
   va_list arglist;
   va_start( arglist, format );
   int r = vfprintf( stream, format, arglist );
   va_end( arglist );
   return r;
}
extern "C" void _assert(const char *__assertion, const char *__file, int __line)
{
	eprintf("Assertion %s failed. %s:%03d\n", __assertion, __file, __line);
	eat_shit_and_die();
}


extern "C" {

/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))


/* types */
typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);

/*
** basic types
*/
#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8


typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

#define LUA_INTEGER	ptrdiff_t
#define LUA_NUMBER		double

/* type of numbers in Lua */
typedef LUA_NUMBER lua_Number;
/* type for integer functions */
typedef LUA_INTEGER lua_Integer;

#define LUA_API extern

/*
** basic stack manipulation
*/
LUA_API int   (lua_gettop) (lua_State *L);
LUA_API void  (lua_settop) (lua_State *L, int idx);
LUA_API void  (lua_pushvalue) (lua_State *L, int idx);
LUA_API void  (lua_remove) (lua_State *L, int idx);
LUA_API void  (lua_insert) (lua_State *L, int idx);
LUA_API void  (lua_replace) (lua_State *L, int idx);
LUA_API int   (lua_checkstack) (lua_State *L, int sz);

LUA_API void  (lua_xmove) (lua_State *from, lua_State *to, int n);


/*
** access functions (stack -> C)
*/

LUA_API int             (lua_isnumber) (lua_State *L, int idx);
LUA_API int             (lua_isstring) (lua_State *L, int idx);
LUA_API int             (lua_iscfunction) (lua_State *L, int idx);
LUA_API int             (lua_isuserdata) (lua_State *L, int idx);
LUA_API int             (lua_type) (lua_State *L, int idx);
LUA_API const char     *(lua_typename) (lua_State *L, int tp);

LUA_API int            (lua_equal) (lua_State *L, int idx1, int idx2);
LUA_API int            (lua_rawequal) (lua_State *L, int idx1, int idx2);
LUA_API int            (lua_lessthan) (lua_State *L, int idx1, int idx2);

LUA_API lua_Number      (lua_tonumber) (lua_State *L, int idx);
LUA_API lua_Integer     (lua_tointeger) (lua_State *L, int idx);
LUA_API int             (lua_toboolean) (lua_State *L, int idx);
LUA_API const char     *(lua_tolstring) (lua_State *L, int idx, size_t *len);
LUA_API size_t          (lua_objlen) (lua_State *L, int idx);
LUA_API lua_CFunction   (lua_tocfunction) (lua_State *L, int idx);
LUA_API void	       *(lua_touserdata) (lua_State *L, int idx);
LUA_API lua_State      *(lua_tothread) (lua_State *L, int idx);
LUA_API const void     *(lua_topointer) (lua_State *L, int idx);


/*
** push functions (C -> stack)
*/
LUA_API void  (lua_pushnil) (lua_State *L);
LUA_API void  (lua_pushnumber) (lua_State *L, lua_Number n);
LUA_API void  (lua_pushinteger) (lua_State *L, lua_Integer n);
LUA_API void  (lua_pushlstring) (lua_State *L, const char *s, size_t l);
LUA_API void  (lua_pushstring) (lua_State *L, const char *s);
LUA_API const char *(lua_pushvfstring) (lua_State *L, const char *fmt,
                                                      va_list argp);
LUA_API const char *(lua_pushfstring) (lua_State *L, const char *fmt, ...);
LUA_API void  (lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
LUA_API void  (lua_pushboolean) (lua_State *L, int b);
LUA_API void  (lua_pushlightuserdata) (lua_State *L, void *p);
LUA_API int   (lua_pushthread) (lua_State *L);


/*
** get functions (Lua -> stack)
*/
LUA_API void  (lua_gettable) (lua_State *L, int idx);
LUA_API void  (lua_getfield) (lua_State *L, int idx, const char *k);
LUA_API void  (lua_rawget) (lua_State *L, int idx);
LUA_API void  (lua_rawgeti) (lua_State *L, int idx, int n);
LUA_API void  (lua_createtable) (lua_State *L, int narr, int nrec);
LUA_API void *(lua_newuserdata) (lua_State *L, size_t sz);
LUA_API int   (lua_getmetatable) (lua_State *L, int objindex);
LUA_API void  (lua_getfenv) (lua_State *L, int idx);

/*
** set functions (stack -> Lua)
*/
LUA_API void  (lua_settable) (lua_State *L, int idx);
LUA_API void  (lua_setfield) (lua_State *L, int idx, const char *k);
LUA_API void  (lua_rawset) (lua_State *L, int idx);
LUA_API void  (lua_rawseti) (lua_State *L, int idx, int n);
LUA_API int   (lua_setmetatable) (lua_State *L, int objindex);
LUA_API int   (lua_setfenv) (lua_State *L, int idx);

/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_newtable(L)		lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

#define lua_strlen(L,i)		lua_objlen(L, (i))

#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))

#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)

}

// extern "C" void luaL_openlib (lua_State *L, const char *libname,
//                               const luaL_Reg *l, int nup) {
//   if (libname) {
//     lua_pushstring(L, libname);
//     lua_gettable(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
//     if (lua_isnil(L, -1)) {  /* no? */
//       lua_pop(L, 1);
//       lua_newtable(L);  /* create it */
//       lua_pushstring(L, libname);
//       lua_pushvalue(L, -2);
//       lua_settable(L, LUA_GLOBALSINDEX);  /* register it with given name */
//     }
//     lua_insert(L, -(nup+1));  /* move library table to below upvalues */
//   }
//   for (; l->name; l++) {
//     int i;
//     lua_pushstring(L, l->name);
//     for (i=0; i<nup; i++)  /* copy upvalues to the top */
//       lua_pushvalue(L, -(nup+1));
//     lua_pushcclosure(L, l->func, nup);
//     lua_settable(L, -(nup+3));
//   }
//   lua_pop(L, nup);  /* remove upvalues */
// }


/*
	Luajit state allocation
*/

extern "C" {
	lua_State *luaL_newstate(void);

	void* memcpy(void* dst, const void* src, unsigned int sz);
	
	void *malloc(size_t size);
	void *realloc(void *ptr, size_t size);
	void free(void* p);
}
// extern "C" {

// typedef struct lua_State lua_State;

// void *lj_alloc_create(void);
// void lj_alloc_destroy(void *msp);
// void *lj_alloc_f(void *msp, void *ptr, size_t osize, size_t nsize);

// static int panic(lua_State *L)
// {
//   const char *s = lua_tostring(L, -1);
//   fputs("PANIC: unprotected error in call to Lua API (", stderr);
//   fputs(s ? s : "?", stderr);
//   fputc(')', stderr); fputc('\n', stderr);
//   fflush(stderr);
//   return 0;
// }

// } // extern "C"


// luajit struct
// struct lua_State {
// 	char unk1[0x40]; // Luajit things
//	
// };




extern "C" void* proxy_malloc(size_t sz){
	return malloc(sz);
}

// struct TObject;
// struct TValue;
// struct cTValue;
// cTValue *lj_meta_tget(lua_State *L, cTValue *o, cTValue *k)

// #define tvchecklive(L, o) \
//   UNUSED(L), lua_assert(!tvisgcv(o) || \
//   ((~itype(o) == gcval(o)->gch.gct) && !isdead(G(L), gcval(o))))

// static LJ_AINLINE void copyTV(lua_State *L, TValue *o1, const TValue *o2)
// {
//   *o1 = *o2; tvchecklive(L, o1);
// }


