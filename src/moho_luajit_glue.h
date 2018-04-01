
/**
luaH functions used by LuaPlus:

Table *luaH_new (lua_State *L, int narray, int lnhash);
void luaH_free (lua_State *L, Table *t);

static int luaH_index (lua_State *L, Table *t, StkId key);
int luaH_next (lua_State *L, Table *t, StkId key);

const TValue *luaH_get (Table *t, const TValue *key);
const TValue *luaH_getnum (Table *t, int key);
const TValue *luaH_getstr (Table *t, TString *key);
TValue *luaH_set (lua_State *L, Table *t, const TValue *key);
TValue *luaH_setnum (lua_State *L, Table *t, int key);


TValue *luaH_setstr (lua_State *L, Table *t, TString *key); Not used?
**/

extern "C" {
  #include "lj_obj.h"
}

#include "ex_core.h"


namespace moho {

extern ::TValue NIL;
extern ::TValue TV_FALSE;
extern ::TValue TV_TRUE;
extern ::TValue tmp_tv;

extern char STR_BUF[128];
 
/** !Not! compatible with GCHeader */
#define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked

typedef unsigned char lu_byte;

// Guard table to pass around in Moho
struct Table {
	GCtab* lj_t;
	int type_tag = 5;

  ::TValue lj_v;

  Table(GCtab* t) {
    lj_t = t;
    settabV(NULL, &lj_v, t);
  }
};

/*
** Union of all Lua values
*/
typedef union {
  GCObject *gc;
  void *p;
  lua_Number n;
  int b;
} Value;


/*
** Tagged Values
*/

/** Luajit: */
/* String object header. String payload follows. */
// typedef struct GCstr {
//   GCHeader;
//   uint8_t reserved; /* Used by lexer for fast lookup of reserved words. */
//   uint8_t unused;
//   MSize hash;   /* Hash of string. */
//   MSize len;    /* Size of string. */
// } GCstr;

/*
** String headers for string table
*/
struct TString{
  ::TValue* lj_v = &_lj_v;
  int type_tag = 4;

  ::TValue _lj_v;

  TString(const GCstr* lj_val) {
    setgcrefp(_lj_v.gcr, (GCstr*)lj_val);
    lj_v->it = LJ_TSTR;

    printf("TString created: %s, 0x%p\n", strdata(gco2str(gcref(lj_v->gcr))), lj_val);
  }

  GCstr* lj() {
    return gco2str(gcref(_lj_v.gcr));
  }
};


struct TValue {
  int tt;
  union {
    ::TValue* lj_p;
    TString* mh_s;
    Table* mh_t;
    float n;
    void* p;
  };

  TValue(const ::TValue* lj_val) {
    tt = itypemap(lj_val);
    // if (tt >= 0)
    //   printf("Unexpected tt for luajit: 0x%x\n", tt);
    // assert(tt < 0);

    // Convert luajit type to moho lua
    switch(tt) {
      case ~LJ_TNIL:
        tt = LUA_TNIL;
        break;
      case ~LJ_TFALSE:
        tt = LUA_TBOOLEAN;
        p = 0;
        break;
      case ~LJ_TTRUE:
        tt = LUA_TBOOLEAN;
        p = (void*)1;
        break;
      case ~LJ_TTAB:
        tt = LUA_TTABLE;
        mh_t = new Table(gco2tab(gcref(lj_val->gcr)));
        break;
      case ~LJ_TNUMX:
        tt = LUA_TNUMBER;
        n = numV(lj_val);
        break;
      // case ~LJ_TLIGHTUD:
      //   tt = LUA_TLIGHTUSERDATA;
      //   p = lightudV(lj_val);
      //   break;
      case ~LJ_TSTR:
        tt = LUA_TSTRING;
        mh_s = new TString(strV(lj_val));
        break;
      // case ~LJ_TPROTO:
      //   tt = 6;
      //   p = protoV(lj_val);
      //   break;
      case ~LJ_TFUNC:
        tt = LUA_TFUNCTION;
        p = funcV(lj_val);
        break;
      case ~LJ_TUDATA:
        tt = LUA_TUSERDATA;
        p = udataV(lj_val);
        break;
      default:
        printf("Unhandled luajit type %d\n", tt);
        eat_shit_and_die();
        lj_p = (::TValue*)lj_val;
    }

    printf("TValue created: %s, 0x%p\n", lj_typename(lj_val), lj_val);
  }

  // void * operator new(size_t size){
  //   void* p = ::operator new (size);
  //   printf("TValue allocated: 0x%p, %d bytes\n", p, size);

  //   return p;
  // }

  // void * operator new(size_t size, TValue* addr){
  //   printf("TValue allocated: 0x%p, %d bytes\n", p, size);

  //   return p;
  // }

  ::TValue* lj() const {
    switch(tt) {
      case LUA_TNIL: 
        return &NIL;
      case LUA_TBOOLEAN:
        if(p == 0) 
          return &TV_FALSE;
        else
          return &TV_TRUE;
      case LUA_TNUMBER:
        setnumV(&tmp_tv, n);
        return &tmp_tv;
      case LUA_TSTRING:
        return mh_s->lj_v;
      case LUA_TTABLE:
        return &mh_t->lj_v;
      case 7://this is LUA_TFUNCTION:
        setfuncV(0, &tmp_tv, (GCfunc*)p);
        return &tmp_tv;
      default:
        printf("Unhandled lua type %d\n", tt);
        eat_shit_and_die();
    }
  }
  operator ::TValue* () {
    return lj();
  }

  operator const ::TValue* () const {
    return lj();
  }

  const char* str() const {
    switch(tt) {
      case LUA_TBOOLEAN:
        return p == 0 ? "false" : "true";
      case LUA_TNUMBER:
        sprintf(STR_BUF, "%f", n);
        return STR_BUF;
      case LUA_TSTRING:
        return strdata(mh_s->lj());
      case LUA_TTABLE:
        sprintf(STR_BUF, "table<0x%p>", mh_t->lj_t);//tabref(lj_val);
        return STR_BUF;
      case LUA_TNIL:
        return "nil";
      default:
        printf("Type unimp: %d\n", tt);
        return "unk";
    }
  }

  int type() { // In lua types
    return tt;
  }

  const char* get_str() const {
    assert(tt == LUA_TSTRING);
    return strdata(mh_s->lj());
  }
};

typedef TValue *StkId;


} // moho

extern "C" void moho_throw_lua_error_traceback(moho::lua_State* L);
extern "C" void moho_throw_lua_error(lua_State* L);

#define Lp_to_L(Lp_L_m) (lua_State*)Lp_L_m->_lua_state->lj
#define CHECK_STACK assert(lua_checkstack(L, 1));

#define LOG_PUSH \
  printf("%s:%03d Pushed %s\n", __FILE__, __LINE__, lua_typename(L, lua_type(L, -1)));

#define CONSERVATIVE_USING_MOHO \
  using moho::LuaState; \
  using moho::LuaObject; \
  using moho::LuaStackObject; \
  using moho::RType; \
  using moho::RRef; \
  using moho::RObject; \
  using M_TV = moho::TValue;