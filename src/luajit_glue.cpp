
#include "ex_core.h"

extern "C" {
#include "lj_obj.h"
#include "lj_meta.h"
#include "lj_err.h"
#include "lj_tab.h"
#include "lj_str.h"
#include "lj_gc.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lj_gc_glue.h"
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

char moho::_STR_BUF[128*3];
int moho::_STR_BUF_I = 0;

bool ext_initialized = 0;

extern "C" moho::LuaState* luaplus_getstateuserdata(lua_State* L);

#include <map>

using std::map;

map<global_State*, LuaState*> g_to_Lp;

// Called when luajit starts a gc run
//   needs to mark objects used in moho luaplus
extern "C" void MOHO_GLUE_gc_mark(global_State* g)
{
  eprintf("MOHO_GLUE_gc_mark!\n");

  lua_State* L = mainthread(g);

  auto it = g_to_Lp.find(g);
  if(it == g_to_Lp.end())
    eat_shit_and_die("No LuaState* ref.");

  moho::LuaState* Lp_L_m = it->second;
  //moho::LuaState* Lp_L_m = luaplus_getstateuserdata(L);

  LuaObject* o = Lp_L_m->used_list;

  while(o && o->m_next) {
    eprintf("MARK 0x%p => 0x%p (%s)\n", o, o->val()->gco(), o->val()->str());

    gc_marktv(g, o->val()->lj());

    o = o->m_next;
  }
}
void moho::root_ref(::GCobj* lj_obj)
{

}
void moho::root_unref(::GCobj* lj_obj)
{

}

extern "C" void log_lua_call(int ordinal, void* x1, void* x2, void* x3, void* x4, void* ret_addr)
{
  eprintf("lua library call 0x%x ( 0x%08x, 0x%08x, 0x%08x, 0x%08x) from 0x%x\n", ordinal, x1,x2,x3,x4, ret_addr);
  fflush(stderr);
}
extern "C" RObject* RType_new_defcon(moho::lua_State* L, RType*);
extern "C" RObject* RType_new_copycon(moho::lua_State* L, RRef*);

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

extern "C" RRef* LuaObject_AssignNewUserData_RRef(LuaObject* This, RRef* rvo, LuaState* Lp_L_m, RRef* ref)
{
  moho::lua_State* L_m = Lp_L_m->_lua_state;

  lua_State* L = Lp_to_L(Lp_L_m);

  RObject* o = RType_new_copycon(L_m, ref);
  assert(tvisudata(--L->top)); // Remove from stack the user data.

  // FIXME (maybe)! Luajit will garbage collect this, unless it is used in lua!

  This->AddToUsedList(Lp_L_m);
  *This->val() = M_TV(L->top);

  rvo->obj = o->data;
  rvo->type = ref->type;

  return rvo;
}

extern "C" RRef* lua_newuserdata_RRef(RRef* rvo, LuaState* Lp_L_m, RRef* ref)
{
  moho::lua_State* L_m = Lp_L_m->_lua_state;

  lua_State* L = Lp_to_L(Lp_L_m);

  RObject* o = RType_new_copycon(L_m, ref);
  //lua_pushlightuserdata(L, rvo); // this happens

  rvo->obj = o->data;
  rvo->type = ref->type;
  return rvo;
}


extern "C" RRef* LuaObject_AssignNewUserData_RTypep(LuaObject* This, RRef* rvo, LuaState* Lp_L_m, RType* type)
{
  moho::lua_State* L_m = Lp_L_m->_lua_state;

  lua_State* L = Lp_to_L(Lp_L_m);

  RObject* o = RType_new_defcon(L_m, type);
  L->top--; // Remove from stack the user data.

  // FIXME (maybe)! Luajit will garbage collect this, unless it is used in lua!

  This->AddToUsedList(Lp_L_m);
  *This->val() = M_TV(L->top);

  rvo->obj = o->data;
  rvo->type = type;

  return rvo;
}

extern "C" RRef* lua_newuserdata_RTypep(RRef* rvo, moho::lua_State* L_m, RType* type)
{
  lua_State* L = (lua_State*)L_m->lj;
  RObject* o = RType_new_defcon(L_m, type);
  //lua_pushlightuserdata(L, rvo); // this happens

  rvo->obj = o->data;
  rvo->type = type;

  eprintf("push RObject\n");

  return rvo;
}

typedef  int (*lib_open_t)(lua_State *L);

extern "C" int glue_lua_load (lua_State *L,
  lua_Reader reader, void *data, const char *chunkname)
{
  eprintf("lua_load 0x%p, 0x%p, 0x%p, %s\n", L, reader, data, chunkname);
  return lua_load(L, reader, data, chunkname);
}

//extern "C" luaL_Reg* gpg_meta_io;

extern "C" void gpg_open_io_and_os(moho::lua_State* L);

extern "C" LuaObject* LuaState_GetGlobals(LuaState* This, LuaObject* rvo)
{
  lua_State* L = Lp_to_L(This);

  lua_pushvalue(L, LUA_GLOBALSINDEX);

  rvo->AddToUsedList(This);

  eprintf("GetGlobals 0x%p -> 0x%p\n", L, (L->top-1)->gcr);
  *rvo->val() = M_TV(--L->top);
  return rvo;
}

extern "C" void LuaState_Init(moho::LuaState* Lp_L_m, int libs) // libs = [0 : nothing, 1 : sandboxed, 2 : everything]
{
  if(!ext_initialized) {
    new (&g_to_Lp) map<global_State*, LuaState*>();
    ext_initialized = 1;
  }

  moho::lua_State* L_m = Lp_L_m->_lua_state;
  lua_State* L = (lua_State*)L_m->lj;

  g_to_Lp[G(L)] = Lp_L_m;

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
    eprintf("Deallocating a pointer not previously allocated!\n");
    eprintf("alloc 0x%p, 0x%p, 0x%x, 0x%x => BAD!\n", ud, ptr, osize, nsize);
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


  eprintf("alloc 0x%p, 0x%p, 0x%x, 0x%x => 0x%p\n", ud, ptr, osize, nsize, ret);
  return ret;
}


extern "C" void _lock(int locknum)
{
  eprintf("Thing!\n");
}
extern "C" void _unlock(int locknum)
{
  eprintf("Thing!\n");
}

extern "C" void* __dllonexit(void* func,  
   void **  pbegin,   
   void **  pend   
   ) {
  eprintf("Thing!\n");
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

  luaL_traceback(L, L, 0, 1);

  const char* traceback = lua_tostring(L, -1);

  eprintf("ERROR: %s\n", msg);
  eprintf("Traceback: %s\n", traceback);

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

  // eprintf("Unable to realloc.\n");
  
  // LuaPlus state is 0x1A8
  
  eprintf("Created luajit state 0x%p\n", L);
  moho::lua_State* L_m = (moho::lua_State*)malloc(0x200);

  memset(L_m, 0, 0x200);


  moho::global_State* G_s = (moho::global_State*)malloc(0x200);

  memset(G_s, 0, 0x200);

  *(moho::lua_State**)(((char*)G_s)+0x44) = L_m;

  lj_state_p = L;

  memcpy(L_m, &L, 4);
  L_m->l_G = G_s;

  lua_atpanic(L, _atpanic);

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

  eprintf("Closing luajit state 0x%x\n", L);
  free(L_m);
  free(G_s);
  lua_close(L);
}

/* Memory allocator */
extern "C" void* glue_lj_alloc(lua_State* L, void* ptr, int osize, int nsize)
{
  // Assume ptr is userdata and will be gc'd

  eprintf("glue_lj_alloc( 0x%x, 0x%x, 0x%x, 0x%x)\n", L, ptr, osize, nsize);

  if(osize)
    return 0;
  else if(nsize)
    return lua_newuserdata(L, nsize);
  else
    eat_shit_and_die("glue_lj_alloc bad.");
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
  L = MAYBE_MOHO_TO_L(L);
  eprintf("luaplus_getstateuserdata 0x%p\n", L);
  lua_getfield(L, LUA_REGISTRYINDEX, "__luaplus_state");
  void* ud = lua_touserdata(L, -1);
  lua_settop(L, -2);

  return (moho::LuaState*)ud;
}

/**
* ==== LuaL functions that are only defines, but LuaPlus used cdecl functions anyway for each and every one, which... come to think of it is incredibly useful for all of these horrible malevolent hacks ====
*/
#undef luaL_getn
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

  eprintf("LuaPlusH_next call: 0x%p, 0x%p, %s\n", L, t, lp_k->val()->str());

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
  eprintf("LuaH call: 0x%x, 0x%p, 0x%p, 0x%p, 0x%p\n", ordinal, L_m->lj, x,y,z);
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
  eprintf("luaH_index 0x%p, 0x%p, %s\n", L, t->lj_t, key->str());
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
  eprintf("luaV_gettable 0x%p :  0x%p [ 0x%p (%s)]\n",
   L, t->lj(), key->lj(), key->str());
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
  eprintf("luaV_settable: table 0x%p gc_next 0x%p, color: 0x%x, dead %d\n",
    tabV(t->lj()), *(void**)tabV(t->lj()), gcV(t->lj())->gch.marked, isdead(G(L), gcV(t->lj())));

  eprintf("luaV_settable: %s, %s\n",
      t->str(), key->str());
	// auto o = lj_meta_tset(L, t->lj(), key->lj());
	// assert(o != NULL);
  eprintf("luaV_settable 0x%p :  0x%p [ 0x%p (%s)] = 0x%p (%s)\n",
   L, t->lj(), key->lj(), key->str(), val_idx->lj(), val_idx->str());
	/* NOBARRIER: lj_meta_tset ensures the table is not black. */
	// copyTV(L, o, val_idx->lj());

eprintf("1\n");
  copyTV(L, L->top++, t->lj());
eprintf("2\n");
  copyTV(L, L->top++, key->lj());
eprintf("3\n");
  copyTV(L, L->top++, val_idx->lj());

  lua_settable(L, -3);
  L->top--;

  eprintf("luaV_settable 0x%p :  0x%p [ 0x%p (%s)] = 0x%p\n", L, t->lj(), key->lj(), key->str(), val_idx->lj());

  // lua_getfield(L, LUA_GLOBALSINDEX, "InitFileDir");
  // const char* s = lua_tostring(L, -1);

  // eprintf("s = %s\n", s);
}

extern "C" TString* luaS_newlstr(lua_State* L, const char* str, int size)
{
  eprintf("luaS_newlstr 0x%p, %s, %d\n", L, str, size);
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