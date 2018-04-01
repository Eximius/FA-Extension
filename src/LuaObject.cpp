

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

extern "C" LuaObject* LuaObject__LuaObject_LuaStackObjectp(LuaObject* This, LuaStackObject* stk)
{
	This->m_state = stk->state;
	This->AddToUsedList(This->m_state);

	lua_State* L = Lp_to_L(stk->state);

	lua_pushvalue(L, stk->stack_index);
	LOG_PUSH

	*This->val() = M_TV(--L->top);

	return This;
}

extern "C" LuaObject* LuaObject_LuaObject_LuaStatep_idx(LuaObject* This, moho::LuaState* Lp_L_m, int idx)
{
  lua_State* L = Lp_to_L(Lp_L_m);

  printf("LuaObject_idx 0x%p, %d\n", L, idx);
  This->m_state = Lp_L_m;

  lua_pushvalue(L, idx);

  LOG_PUSH

  *This->val() = M_TV(--L->top);

  This->AddToUsedList(This->m_state);
  return This;
}

extern "C" void LuaObject_Del(LuaObject* This)
{
  if(This->m_state)
    This->RemoveFromUsedList();
}

extern "C" void LuaObject_Register(LuaObject* This, const char* func_name, void* func, int nups)
{

  printf("Register %s [ %s ] -> 0x%p\n", This->val()->str(), func_name, func);
  lua_State* L = (lua_State*)This->m_state->_lua_state->lj;

  copyTV(L, L->top++, This->val()->lj());

  lua_pushlstring(L, func_name, strlen(func_name));

  lua_pushcclosure(L, (lua_CFunction)func, nups);

  lua_settable(L, -3);
  lua_settop(L, -2);
}

extern "C" LuaStackObject* LuaObject_PushStack_LuaStatep(LuaObject* This, LuaStackObject* rvo, moho::LuaState* Lp_L_m)
{
  lua_State* L = (lua_State*)Lp_L_m->_lua_state->lj;

  CHECK_STACK

  copyTV(L, L->top++, ((M_TV*)&This->type)->lj());

  LOG_PUSH

  return rvo;
}

extern "C" void LuaObject_PushStack_lua_Statep(LuaObject* This, moho::lua_State* L_m)
{
  lua_State* L = (lua_State*)L_m->lj;

  CHECK_STACK

  copyTV(L, L->top++, ((M_TV*)&This->type)->lj());

  LOG_PUSH
}

extern "C" LuaObject* LuaObject_op_subscript_cstr(LuaObject* This, LuaObject* rvo, const char* cstr)
{
  lua_State* L = (lua_State*)This->m_state->_lua_state->lj;

  printf("op_sub: 0x%p 0x%p (%s) [ \"%s\" ]\n", L, ((M_TV*)&This->type)->lj(), ((M_TV*)&This->type)->str(), cstr);

  CHECK_STACK

  copyTV(L, L->top++, ((M_TV*)&This->type)->lj());

  //LOG_PUSH

  lua_pushstring(L, cstr);
  lua_rawget(L, -2);

  LOG_PUSH

  memset(rvo, 0, 0x14);

  rvo->AddToUsedList(This->m_state);
  //rvo->m_state = This->m_state;

  TValue* lj_v = (TValue*)malloc(0x8);
  copyTV(L, lj_v, L->top-1);
  *rvo->val() = M_TV(lj_v);

  lua_settop(L, -3);

  return rvo;
}

extern "C" LuaObject* LuaObject_op_eq_StackObject(LuaObject* This, LuaStackObject* stk)
{
  This->RemoveFromUsedList();
  This->m_state = stk->state;
  This->AddToUsedList(This->m_state);

  lua_State* L = Lp_to_L(This->m_state);

  lua_pushvalue(L, stk->stack_index);

  *This->val() = M_TV(--L->top);


  printf("LuaObject=(StackObject) 0x%p = %s\n", This->val()->str());

  return This;
}

extern "C" const char* LuaObject_GetString(LuaObject* This)
{
  return This->val()->get_str();
}