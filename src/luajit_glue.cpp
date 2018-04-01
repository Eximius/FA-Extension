
#include "ex_core.h"

extern "C" {
#include "lj_obj.h"
#include "lj_meta.h"
#include "lj_err.h"
#include "lj_tab.h"
#include "lj_str.h"
#include "lj_gc.h"
#include "lualib.h"
}

namespace moho {
  #include "moho.h"
}
#include "moho_luajit_glue.h"

CONSERVATIVE_USING_MOHO

::TValue moho::NIL = { 0xFFFFFFFF00000000ul };
::TValue moho::TV_FALSE = { 0xFFFFFFFE00000000ul };
::TValue moho::TV_TRUE = { 0xFFFFFFD00000000ul };
::TValue moho::tmp_tv;

char moho::STR_BUF[128];

extern "C" RObject* RType_new_defcon(moho::lua_State* L, RType*);
extern "C" RRef* RType_new_copycon(moho::lua_State* L, RRef*);

extern "C" moho::LuaState* luaplus_getstateuserdata(lua_State* L);

// extern "C" cTValue *lj_meta_tget(lua_State *L, cTValue *o, cTValue *k);
// extern "C" TValue *lj_meta_tset(lua_State *L, cTValue *o, TValue *k);
// LJ_FUNCA int lj_tab_next(lua_State *L, GCtab *t, TValue *key);

/** Originally static in luajit */
extern "C" uint32_t keyindex(lua_State *L, GCtab *t, cTValue *key);

extern "C" lua_State* lj_state_p;
extern "C" lua_State *luaL_newstate(void);
/**
  ==== Moho functions reimplemented ====
*/

extern "C" void LuaObject_AssignNewUserData_RRef(LuaObject* This, RRef* rvo, lua_State* L, RRef* ref)
{
  eat_shit_and_die(); 
}

extern "C" void lua_newuserdata_RRef(RRef* rvo, lua_State* L, RRef* ref)
{
  eat_shit_and_die(); 
}


extern "C" void LuaObject_AssignNewUserData_RTypep(LuaObject* This, RRef* rvo, lua_State* L, RType* type)
{
  eat_shit_and_die(); 
}

extern "C" void lua_newuserdata_RTypep(RRef* rvo, moho::lua_State* L_m, RType* type)
{
  lua_State* L = (lua_State*)L_m->lj;
  RObject* o = RType_new_defcon(L_m, type);

  rvo->obj = o->data;
  rvo->type = type;

  printf("push RObject\n");
  //lua_pushlightuserdata(L, rvo);
}

typedef  int (*lib_open_t)(lua_State *L);

extern "C" int glue_lua_load (lua_State *L,
  lua_Reader reader, void *data, const char *chunkname)
{
  printf("lua_load 0x%p, 0x%p, 0x%p, %s\n", L, reader, data, chunkname);
  return lua_load(L, reader, data, chunkname);
}

//extern "C" luaL_Reg* gpg_meta_io;

extern "C" void gpg_open_io_and_os(moho::lua_State* L);

extern "C" LuaObject* LuaState_GetGlobals(LuaState* This, LuaObject* rvo)
{
  lua_State* L = Lp_to_L(This);

  lua_pushvalue(L, LUA_GLOBALSINDEX);

  rvo->m_state = This;
  rvo->AddToUsedList(rvo->m_state);

  printf("GetGlobals 0x%p -> 0x%p\n", L, (L->top-1)->gcr);
  *rvo->val() = M_TV(--L->top);
  return rvo;
}

extern "C" void LuaState_Init(moho::LuaState* Lp_L_m, int libs) // libs = [0 : nothing, 1 : sandboxed, 2 : everything]
{
  moho::lua_State* L_m = Lp_L_m->_lua_state;
  lua_State* L = (lua_State*)L_m->lj;

  lib_open_t safe_libs[] = {
    luaopen_base,
    luaopen_math,
    luaopen_string,
    luaopen_table, 
    luaopen_debug
  };

  lib_open_t unsafe_libs[] = {
    luaopen_io
  };

  for(auto& f : safe_libs) {
    lua_pushcfunction(L,f);
    lua_call(L,0,0);
  }


  if(libs == 2) {
    for(auto& f : unsafe_libs) {
      lua_pushcfunction(L,f);
      lua_call(L,0,0);
    }
    gpg_open_io_and_os(L_m);
    //luaL_setfuncs(L, gpg_meta_io, 1)
  }

  TValue* o = (TValue*)(((char*)L_m) + 0x1A8);
  settabV(L, o, tabref(L->env));


  new (&L_m->globals) M_TV(o);

  // set userdata (as in the reference to LuaState*) in moho::lua_State
  //*(void**)(((char*)L_m)+0x44) = Lp_L_m;
  lua_pushlightuserdata(L, Lp_L_m);
  lua_setfield(L, LUA_REGISTRYINDEX, "__luaplus_state");
  // luaopen_io(L);
  // luaopen_os(L);
  // luaopen_package(L);
  // luaopen_bit(L);
  // luaopen_jit(L);
  // luaopen_ffi(L);
}

#include <set>

std::set<void*>* allocated = 0;
void* an_alloc(void* ud, void* ptr, int osize, int nsize)
{
  if(allocated == 0) {
    allocated = new std::set<void*>();
  }
  if(ptr && !allocated->count(ptr)) {
    printf("Deallocating a pointer not previously allocated!\n");
    printf("alloc 0x%p, 0x%p, 0x%x, 0x%x => BAD!\n", ud, ptr, osize, nsize);
    return 0;
    //eat_shit_and_die();
  }
  void* ret = 0;
  if(nsize == 0) {
    allocated->erase(ptr);
    free(ptr);
  }
  else {
    void* new_p = malloc(nsize);

    memcpy(new_p, ptr, nsize > osize ? osize : nsize);

    allocated->erase(ptr);
    allocated->insert(new_p);

    ret = new_p;
  }


  printf("alloc 0x%p, 0x%p, 0x%x, 0x%x => 0x%p\n", ud, ptr, osize, nsize, ret);
  return ret;
}


extern "C" void _lock(int locknum)
{
  printf("Thing!\n");
}
extern "C" void _unlock(int locknum)
{
  printf("Thing!\n");
}

extern "C" void* __dllonexit(void* func,  
   void **  pbegin,   
   void **  pend   
   ) {
  printf("Thing!\n");
  return func;
}

moho::lua_State* moho_guard(lua_State* L) {
  lua_getfield(L, LUA_REGISTRYINDEX, "__moho_state");
  auto p = (moho::lua_State*)lua_touserdata(L, -1);
  lua_settop(L, -1);
  return p;
}

int _atpanic(lua_State* L) {
  const char* msg = lua_tostring(L, -1);

  printf("ERROR: %s\n", msg);
  moho_throw_lua_error(L);
}
/*
  ==== Luajit state allocation/fiddling ====
*/
extern "C" moho::lua_State* gpg_luaL_newstate()
{
  lua_State* L;
  //L = lua_newstate((lua_Alloc)an_alloc, 0);
  L = luaL_newstate();
  // if (fois == 2)
  //   L = lua_newstate((lua_Alloc)an_alloc, 0);
  // else
  //   L = luaL_newstate();

  // Luajit lua_State of a thread is 48 (0x30) bytes

  // lua_State* L_n = (lua_State*)realloc(L, 0x100);

  // if (L_n)
  //  return L_n;

  // printf("Unable to realloc.\n");
  
  // LuaPlus state is 0x1A8
  
  printf("Created luajit state 0x%p\n", L);
  moho::lua_State* L_m = (moho::lua_State*)malloc(0x200);

  memset(L_m, 0, 0x200);


  moho::global_State* G_s = (moho::global_State*)malloc(0x200);

  memset(G_s, 0, 0x200);

  *(moho::lua_State**)(((char*)G_s)+0x44) = L_m;

  lj_state_p = L;

  memcpy(L_m, &L, 4);
  L_m->l_G = G_s;

  //lua_atpanic(L, _atpanic);

  lua_pushlightuserdata(L, G_s);
  lua_setfield(L, LUA_REGISTRYINDEX, "__moho_global_state");

  lua_pushlightuserdata(L, L_m);
  lua_setfield(L, LUA_REGISTRYINDEX, "__moho_state");

  return L_m;
}

extern "C" void glue_lua_close(lua_State* L)
{
  moho::LuaState* Lp_L_m = luaplus_getstateuserdata(L);
  moho::lua_State* L_m = Lp_L_m->_lua_state;
  moho::global_State* G_s = L_m->l_G;

  printf("Closing luajit state 0x%x\n", L);
  free(L_m);
  free(G_s);
  lua_close(L);
}

/* Memory allocator */
extern "C" void* glue_lj_alloc(lua_State* L, void* ptr, int osize, int nsize)
{
  // Assume ptr is userdata and will be gc'd

  printf("glue_lj_alloc( 0x%x, 0x%x, 0x%x, 0x%x)\n", L, ptr, osize, nsize);

  return lua_newuserdata(L, nsize);
  // if(nsize)
  //   lua_newuserdata(L, nsize);
  //return lj_mem_realloc(L, ptr, osize, nsize);
  //return G(L)->allocf(G(L)->allocd, ptr, osize, nsize);
}

/**
  Gets LuaPlus per-thread "user-data"
  only used for LuaState* in moho
*/
extern "C" moho::LuaState* luaplus_getstateuserdata(lua_State* L)
{
  printf("luaplus_getstateuserdata 0x%p\n", L);
  lua_getfield(L, LUA_REGISTRYINDEX, "__luaplus_state");
  void* ud = lua_touserdata(L, -1);
  lua_settop(L, -2);

  return (moho::LuaState*)ud;
}

/**
* ==== LuaL functions that are only defines, but LuaPlus used cdecl functions anyway for each and every one, which... come to think of it is incredibly useful for all of these horrible malevolent hacks ====
*/

extern "C" int luaL_getn(lua_State* L, int idx)
{
  // #define luaL_getn(L,i)          ((int)lua_objlen(L, i))
  return (int)lua_objlen(L, idx);
}
/**
* ==== Begin luaH/luaV functions ==== 
*/

using moho::Table;
using moho::StkId;
using moho::TString;

using M_TV = moho::TValue;

extern "C" bool LuaPlusH_next(moho::LuaState* Lp_L_m, LuaObject* lp_t, LuaObject* lp_k, LuaObject* lp_v)
{
  lua_State* L = (lua_State*)Lp_L_m->_lua_state->lj;

  cTValue *t = lp_t->val()->lj();
  TValue kv[2];
  kv[0] = *lp_k->val()->lj();

  printf("LuaPlusH_next call: 0x%p, 0x%p, %s\n", L, t, lp_k->val()->str());

  api_check(L, tvistab(t));
  bool more = lj_tab_next(L, tabV(t), kv);

  if(more) {
    *lp_k->val() = M_TV(kv);
    *lp_v->val() = M_TV(kv+1);
  }
  return more;
}

extern "C" void log_luaH(int ordinal, void* ret_adr, moho::lua_State* L_m, void* x, void* y, void* z)
{
  printf("LuaH call: 0x%x, 0x%p, 0x%p, 0x%p, 0x%p\n", ordinal, L_m->lj, x,y,z);
}
extern "C" Table *luaH_new (lua_State *L, int narray, int lnhash) 
{
  auto lj_t = lj_tab_new(L, narray+1, lnhash);
  return new moho::Table(lj_t); 
}

extern "C" void luaH_free (lua_State *L, Table *t)
{
  lj_tab_free(G(L), t->lj_t);
  free(t);
}

extern "C" int luaH_index (lua_State *L, Table *t, M_TV* key)
{
  printf("luaH_index 0x%p, 0x%p, %s\n", L, t->lj_t, key->str());
  return keyindex(L, t->lj_t, key->lj());
}

extern "C" int luaH_next (lua_State *L, Table *t, M_TV* key)
{
  return lj_tab_next(L, t->lj_t, key->lj());
}

extern "C" const M_TV *luaH_get (Table *t, const M_TV *key)
{
  // FIXME!: using global lua_State*
  return new M_TV(lj_tab_get(lj_state_p, t->lj_t, key->lj()));
}

extern "C" const M_TV *luaH_getnum (Table *t, int key)
{
  return new M_TV(lj_tab_getinth(t->lj_t, key));
}

extern "C" const M_TV *luaH_getstr (Table *t, TString *key)
{
  return new M_TV(lj_tab_getstr(t->lj_t, key->lj()));
}

extern "C" M_TV *luaH_set (lua_State *L, Table *t, const M_TV *key)
{
  return new M_TV(lj_tab_set(L, t->lj_t, key->lj())); 
}

extern "C" M_TV *luaH_setnum (lua_State *L, Table *t, int key)
{
  return new M_TV(lj_tab_setinth(L, t->lj_t, key));
}

extern "C" 
const M_TV *luaV_gettable(lua_State *L, const M_TV *t, const M_TV *key, int loop)
{
  const TValue* v = lj_meta_tget(L, t->lj(), key->lj());
  return new M_TV(v);
}

static TValue *stkindex2adr(lua_State *L, int idx)
{
  if (idx > 0) {
    TValue *o = L->base + (idx - 1);
    return o < L->top ? o : niltv(L);
  } else {
    api_check(L, idx != 0 && -idx <= L->top - L->base);
    return L->top + idx;
  }
}

extern "C" void luaV_settable (lua_State *L, const M_TV *t, M_TV *key, StkId val_idx)
{
	auto o = lj_meta_tset(L, t->lj(), key->lj());
	assert(o != NULL);
	/* NOBARRIER: lj_meta_tset ensures the table is not black. */
	copyTV(L, o, val_idx->lj());

  printf("luaV_settable 0x%p :  0x%p [ 0x%p (%s)] = 0x%p\n", L, t->lj(), key->lj(), key->str(), val_idx->lj());

  // lua_getfield(L, LUA_GLOBALSINDEX, "InitFileDir");
  // const char* s = lua_tostring(L, -1);

  // printf("s = %s\n", s);
}

extern "C" TString* luaS_newlstr(lua_State* L, const char* str, int size)
{
  printf("luaS_newlstr 0x%p, %s, %d\n", L, str, size);
  GCstr* s = lj_str_new(L, str, size);
  return new TString(s);
}

/* Fuck you LuaPlus */
// extern "C" void luaG_runerror(lua_State *L, const char *fmt, ...){
//   luaL_error(lua_State *L, const char *fmt, ...);
// } // Done by assembly jmp

extern "C" void luaG_typeerror(moho::lua_State* L_m, M_TV* o,  const char* attempt)
{
  lj_err_optype((lua_State*)L_m->lj, o->lj(), LJ_ERR_OPCALL);
}