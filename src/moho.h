/**
  MohoEngine.dll Disassembling notes
*/

// Nothing here is concrete.

typedef unsigned int uint;

struct string
{
	// 0x1c bytes
	void* ptr1;
	void* m_data; // SSO space start
	void* ptr3;
	void* ptr4;
	void* ptr5;

	// at 0x14
	int size;
	// at 0x18
	int capacity; // size < 0x10 => SSO

#ifdef CXX_BUILD
	const char* data()
	{
		if(size < 0x10)
			return (const char*)&m_data;
		else
			return (const char*)m_data;
	}
#endif
};

struct vector
{
	void* unknown1;
	void** objects_begin;
	void** objects_end;
	void** objects_capacity_end;

#ifdef CXX_BUILD
	void* operator[](int index)
	{
		if(index >= size())
			return 0;
		return objects_begin[index];
	}

	int size()
	{
		return objects_end - objects_begin;
	}
#endif
};

struct list // probably not from visual c++, but made by gpg
			// considering, it's not capacity based, probably made by gpg
{
	void* objects_start; // 0 if empty
	void* objects_end; 
};

typedef int SOCKET;
// GPGCore

struct Stream
{

};
struct PipeStream // : Stream
{
	// 0x48 bytes
	char iDontSeeANeedToKnowTheStructure[0x48];
};

struct gpg_mutex
{
	int vooodoo;
};
// LuaPlus
struct lua_state
{
	int dummy;
};

struct lua_State;
struct LuaState;

typedef unsigned int lu_int32;
struct GCObject;

#ifdef CXX_BUILD
	struct TValue;
#endif

struct LuaObject
{
	// 0x14 bytes

/*	
	Definition in LuaPlus:
	LuaObject* m_next;		   // only valid when in free list
	LuaObject* m_prev;		   // only valid when in used list
*/
	LuaObject* m_next; 
	LuaObject* m_prev;
	LuaState* m_state;

	// struct TObject {
	int type;
	void* p_union; // = RRef* if type == 8
	// } // TObject
	
	/* Types:
		-1 - None
		0 - Nil
		1 - Boolean
		2 - LightUserData // || UserData
		3 - Number // || Integer
		4 - String
		5 - Table
		6 - CFunction
		7 - Function
		8 - UserData
	*/

#ifdef CXX_BUILD
	TValue* val() {
		return (TValue*)&type;
	}

	void __thiscall __AddToUsedList(LuaState*);

	void AddToUsedList(LuaState* state) {
		printf("AddToUsedList 0x%p, 0x%p\n", this, state);
		__AddToUsedList(state);
	}
	void RemoveFromUsedList() {
		printf("RemoveFromUsedList 0x%p, 0x%p\n", this, m_state);
		if(m_state) {
			m_next->m_prev = m_prev;
			m_prev->m_next = m_next;
			m_prev = (LuaObject*)-1;
			m_next = (LuaObject*)-1;
		}
	}
#endif
};

struct LuaState
{
	lua_State* _lua_state;
	void* fuckknows1; // init -> 0
	void* knowsfuck2;
	// void* fnows3kuck;
	// void* important1;

	// at 0xC
	LuaObject obj;

	// at 0x20
	LuaState* m_rootState;  // can point to this LuaState*

	// at 0x24

	// likely LuaObject* dlist? {
		LuaObject* used_list; // -> list_ptr2 // gc_next?
		
		int zero1; // init -> 0 

		// Likely dummy list_end node {
			// at 0x2c
			void* list_ptr2; // -> 0

			// at 0x30
			void* list_ptr3; // -> list_ptr1 // prev
		// } list_end
	// } gc_list;
};

struct global_State 
{
	// struct stringtable:
	GCObject **hash;
	lu_int32 nuse;  /* number of elements */
	int size;

	// at 0x24
	int sz2; //?
	void* obj1;
	// at 0x2c
	int sz1; //?

	// at 0x44
	lua_State* mainthread; // 99%

	// at 0x14c
	void* gc_function; // void (*)(void*)

	// at 0x138
	void* _LuaPlus_ud;
	void* _LuaPlus_realloc;
	void* _LuaPlus_free;

	// at 0x148
	void* global_user_data;

};

/* global_State as defined in LuaPlus 5.1 */
/*
** `global state', shared by all threads of this state
*/
// typedef struct global_State {
//   stringtable strt;  /* hash table for strings */
     // at 0xC
//   lua_Alloc frealloc;  /* function to reallocate memory */
	// at 0x10
//   void *ud;         /* auxiliary data to `frealloc' */
//   lu_byte currentwhite;
//   lu_byte gcstate;  /* state of garbage collector */
//   int sweepstrgc;  /* position of sweep in `strt' */
//   GCObject *rootgc;  /* list of all collectable objects */
	// at 0x20
//   GCObject **sweepgc;  /* position of sweep in `rootgc' */
//   GCObject *gray;  /* list of gray objects */
//   GCObject *grayagain;  /* list of objects to be traversed atomically */
//   GCObject *weak;  /* list of weak tables (to be cleared) */
	// at 0x30
//   GCObject *tmudata;  /* last element of list of userdata to be GC */
//   Mbuffer buff;  /* temporary buffer for string concatentation */ // size 0xC
	// at 0x40
//   lu_mem GCthreshold;
//   lu_mem totalbytes;  /* number of bytes currently allocated */
//   lu_mem estimate;  /* an estimate of number of bytes actually in use */
//   lu_mem gcdept;  /* how much GC is `behind schedule' */
//   int gcpause;  /* size of pause between successive GCs */
//   int gcstepmul;  /* GC `granularity' */
//   lua_CFunction panic;  /* to be called in unprotected errors */
//   TValue l_registry;
//   struct lua_State *mainthread;
//   UpVal uvhead;  /* head of double-linked list of all open upvalues */
//   struct Table *mt[NUM_TAGS];  /* metatables for basic types */
//   TString *tmname[TM_N];  /* array with tag-method names */
// #if LUAPLUS_EXTENSIONS
//   void (*userGCFunction)(void*);
//   void* gchead_next;		   // only valid when in free list
//   void* gchead_prev;		   // only valid when in used list
//   void* gctail_next;		   // only valid when in free list
//   void* gctail_prev;		   // only valid when in used list
//   void (*loadNotifyFunction)(lua_State *L, const char *);
// #endif /* LUAPLUS_EXTENSIONS */
// } global_State;


struct GCObject;
struct TObject
{
	int type;
	void* p_union;
};

struct CallInfo;

typedef unsigned char lu_byte;

struct lua_State
{
	// Assuming LuaPlus was built with !LUA_4REFCOUNT
	union{
		GCObject *next; 
		void* lj;
	};

	lu_byte tt; lu_byte marked; // CommonHeader

	lu_byte status;

	// at 0x8
	void* stack[2];
	//StkId top;  /* first free slot in the stack */
	//StkId base;  /* base of current function */

	// at 0x10 
	global_State *l_G;

	CallInfo* ci;

	// at 0x18
	int some_size;
	void* mem_ptr;
	// at 0x20
	int mem_size; //lua_State* _main_state; // ?

	void* unk2[3];

	// at 0x30
	TObject globals;

	// at 0x40
	void* unk1;

	// at 0x44
	void* user_data; // === LuaState*, because fucking reasons

};
/** LuaPlus 5.1 source: */
/*
** `per thread' state
*/
// struct lua_State {
//   CommonHeader;
	// at 0x8
//   lu_byte status;
	// at 0xC
//   StkId top;  /* first free slot in the stack */
	// at 0x10
//   StkId base;  /* base of current function */
//   global_State *l_G;
//   CallInfo *ci;  /* call info for current function */
//   const Instruction *savedpc;  /* `savedpc' of current function */
	// at 0x20
//   StkId stack_last;  /* last free slot in the stack */
//   StkId stack;  /* stack base */
//   CallInfo *end_ci;  /* points after end of ci array*/
//   CallInfo *base_ci;  /* array of CallInfo's */
//   int stacksize;
//   int size_ci;  /* size of array `base_ci' */
//   unsigned short nCcalls;  /* number of nested C calls */
//   unsigned short baseCcalls;  /* nested C calls when resuming coroutine */
//   lu_byte hookmask;
//   lu_byte allowhook;
//   int basehookcount;
//   int hookcount;
//   lua_Hook hook;
//   TValue l_gt;  /* table of globals */
//   TValue env;  /* temporary place for environments */
//   GCObject *openupval;  /* list of open upvalues in this stack */
//   GCObject *gclist;
//   struct lua_longjmp *errorJmp;  /* current error recover point */
//   ptrdiff_t errfunc;  /* current error handling function (stack index) */
// #if LUA_MEMORY_STATS
// #ifdef _DEBUG
//   const char* allocName;
// #endif /* _DEBUG */
//   unsigned int allocFlags;
// #endif /* LUA_MEMORY_STATS */
// };

struct RType // at least 0x58 size
{
	int unk1;
	int unk2;
	int type_size;

	// at 0x54
	void* defcon; // void (*)(void* rvo, void* This)

	// at 0x5c
	int flags; // ?
};

struct RObject // ? (created from RType)
{
	// Stored in LUA as userdata (8)
	struct {
		void* unk1; // = global_State[0x14]
		char type_tag = 8;
		bool something; // ? ; = RType->flags & 2
		char pad[2]; // ?

		void* crap; // = global_State[0x118]

		RType* type;
	} _rtype; // Type-info data (0x10 size)

	char data[];
	// char[type_size]; of data
};

struct RRef
{
	void* obj;
	RType* type;
};

struct LuaStackObject
{
	// 0x8 bytes
	LuaState* state;
	int stack_index;
};


// Moho

struct linked_list
{
	void* not_sure;
};
struct moho_set
{
	int set_base; // integer_base >> 5 (bits in dword)
	int unknown2;
	uint* items_begin;
	uint* items_end;
	uint* items_capacity_end;
	void* unknown6; 
	void* unknown7; // Used as memory for Short Set 'Optimization'
	void* unknown8;

#ifdef CXX_BUILD
	void add(int item)
	{
		items_begin[item>>5] |= 1 << (item & 0x1f);
	}

	void remove(int item)
	{
		items_begin[item>>5]  &= ~( 1 << (item & 0x1f) );
	}

	bool operator[](int item)
	{
		return items_begin[item>>5] & (1 << (item & 0x1f));
	}
#endif
};

struct Unknown1 // from WLD_SetupSessionInfo
{
	// 0xA4 bytes
	void* vtable;

	void* self1;
	void* self2;

	// at 0xC
	string lua_gameMods;
	// at 0x28
	string str2;

	// at 0x60
	int unknown1;
	int unknown2;
	int unknown3; // = -1
	// at 0x6C
	string str3;
	// at 0x88
	bool boolean1;
	char _pad1[3];
	int unknown4; 
};

struct IClientManager
{
	void* vtable;
};

struct RRuleGameRules
{
	int dummy;
};
struct CWldMap
{
	void* zero1;
	void* zero2;
	void* zero3;
};

struct SWldSessionInfo
{
	// 0x30 bytes
	string map_name;

	// at 0x1C
	int unknown1;

	// at 0x20
	int unknown2;
	bool b1; // = true
	bool b2;
	bool b3;
	char _pad1;

	IClientManager* clientManager;
	int unknown4; // = 255 possibly cmdSourceIndex
};
struct UserArmy
{
	void* unknown1; // vtable?

	string name;

	// at 0x20
	string nickname;

	#ifndef FORGED_ALLIANCE
	char datas[0xf4];
	#else
	char datas[0xec];
	#endif
	// at 0x130 Moho | at 0x128 FA
	moho_set mValidCommandSources;
	// at 0x148 FA
	uint color;
	// at 0x16C FA
	int faction;
};
struct SimArmy
{
#ifdef FORGED_ALLIANCE
	// Forged Alliance Code
	void* vtable;
	int unknown3;
	int unknown4;

	string name;
	string nickname;

	char datas[0xec];
	// at 0x138 Moho | at 0x130 FA
	moho_set mValidCommandSources;

	// at 0x158 FA
	string mArmyType; //? 'human' for players

	// at 0x1C4 FA
	struct{
		float funknown1;
		float funknown2;
	} float_struct;

	// at 0x1F0 FA
	int unknown1;
	int unknown2;
	// at 0x1F8 FA
	string unknown5;
#else
	// Moho Code

	// at 0x138 Moho | at 0x130 FA
	moho_set mValidCommandSources;
#endif
};

struct Sim
{
	// 0xAF8 bytes

#ifdef FORGED_ALLIANCE
	char datas[0x90C];
	// at 0x91C Moho | at 0x90C FA
	vector armies;// <class Moho::SimArmy *>
	// at 0x93C Moho | at 0x92C FA
	int ourCmdSource; // possibly just current in simulation.
	// at 0x984 FA
	void* thing;
#else
#endif
};
struct CWldSession 
{
	// 0x508 bytes
	// Information about values is from constructor.

	CWldSession* self1; // = this
	CWldSession* self2; // = this

	void* self_weird1; // = this + 0x8
	void* self_weird2; // = this + 0x8

	// at 0x10
	LuaState* state; // sim? set from constructor argument
	void* zero1;

	RRuleGameRules* rules;
	CWldMap* map;

	// at 0x20
	void* zero2;
	void* zero3;
	string map_name;

	char stuff[0x3ac];

	// at 0x3f0
	list /*?*/ armies; // <UserArmy*>

	// at 0x470
	vector cmdSources; // <SSTICommandSource>

	// at 0x480
	int ourCmdSource;

	bool isReplay;
	bool isBeingRecorded;
	bool isMultiplayer;

	// at 0x487
	bool bool1; // set as true in Constructor.

	int focusArmyIndex; // focused army, -1 = observer

	bool isGameOver;

	// at 0x4d4
	bool cheatsEnabled;
};

struct STIDriver // : ISTIDriver
{
	// 0x230 bytes
};

struct STIMap
{
	// 0x1548 bytes
};

struct SSTICommandSource
{
	// 0x24 bytes

	int index;
	string name;
	int protocol;
};

struct CPushTask
{
	char datas[0x24];
};

typedef CPushTask CPullTask;

struct INetConnector
{
	void* vtable;
};
struct CLobby
{
	// 0xC8 bytes

	// at 0x20
	LuaObject unknown0;

	int unknown1;
	int unknown2;
	int unknown3;
	int unknown4;
	// at 0x44
	CPushTask pushTask;

	// at 0x60
	CPullTask pullTask;

	// at 0x84
	bool hosted_or_joined; // Has CLobby been used to host/join yet?

	// at 0x88
	void* not_host; // 0 if we're host

	// at 0x90
	string our_name;
	// at 0xAC
	int our_UID;

	// at 0xB0
	struct
	{
	  INetConnector* first_element;
	  INetConnector* last_element; // It will probably always be our CNetXXXConnector
	} peer_list; // Probably singly-linked list
};

/// Tres Importante
struct sub_10392B10_ret
{
	// 0x20 bytes

	void* zero1; // self in CLobbyPeer.unknown2
	void* zero2; // self in CLobbyPeer.unknown2
	void* zero3; // self in CLobbyPeer.unknown2
	int unknown1;

	// at 0x10
	char one1;
	char zero4; // 1 in CLobbyPeer.unknown2
};
struct CLobbyPeer
{
	// 0x50 bytes
	CLobbyPeer* next; // Doubly linked list pointers
	CLobbyPeer* prev; 

	string			  playerName;

	// at 0x24
	int ownerID; // User UID
	int constant1;
	short constant2;
	char _pad1[2];

	// at 0x30
	int constant3;	// enum?
	float constant4; // = 0
	int constant5;
	int unknown1;

	// at 0x40
	sub_10392B10_ret* unknown2; // made in sub_10394180
	int zero1;
	int cmdSourceIndex; // == 255 => Unassigned
	int playerNo;

	/*
		constant1, constant2, constant3, constant5
		specially define the CLobbyUser

		constant3:
		1 - Host for a lobby we joined?
		3 - Peer who connected to us?
		6 - Game Peer of a Launched Game?
	*/
};

struct CClientManagerImpl // : IClientManager
{
	// 0x184D0 bytes
	void* vtable;

	// at 0x40C
	gpg_mutex mLock;

	// at 0x420
	list mClients; // <IClient*>
	INetConnector* mConnector;

	// at 0x434
	bool mWeAreReady;
	bool mEveryoneIsReady;
	char _pad1[2];

	int mDispatchedBeat;
	int mAvailableBeat;
	int mFullyQueuedBeat;
	int mPartiallyQueuedBeat;
	int mGameSpeedClock;
	int mGameSpeedRequester; // misspelling? :D
	int mGameSpeed;
};

struct CClientBase // : IClient
{
	// 0xD8 bytes
	void* vtable;
	string mNickname;
	// at 0x20
	int mIndex; // client index 
	int mUID; 
	IClientManager* clientManager;

	// at 0x30
	/**
		+-------------------------------------------------------+
		|                                                       |
		|    GOOD NEWS EVERYONE, IT'S A MOHO::SET***            |
		|                                                       |
		|    ( It's still a really weird structure              |
		|      due to the magical optimizations                 |
		|      possible for AABB Sets via bit manipulation. )   |
		|                                                       |
		|    *** Probably                                       |
		+-------------------------------------------------------+

		Actually, a shitty bitvector
	*/
	struct NoFuckingClue
	{
		int set_base; // integer_base >> 5 (bits in dword)
		int unknown2;
		void* memory;
		void* memory_end;
		void* unknown5;
		void* unknown6;
		void* unknown7;
		void* unknown8;
	} thisIsTheWeirdestStructureIHaveSeen;

	//vector mValidCommandSources; // <int>?

	//int unknwon3;
	//int unknown4;
	//int unknwon5;
	//int unknown6;
	
	// at 0x50
	int mCommandSource;
	bool mReady;
	char _pad1[3];

	PipeStream mPipe;

	// at 0xA0
	int mQueuedBeat;
	int mDispatchedBeat;
	int mAvailableBeatRemote;
	vector mLatestAckReceived;
	int mLatestBeatDispatchedRemote;
	// at 0xC0
	bool mEjectPending;
	bool mEjected;
	char _pad2[2];
	list mEjectRequests; // <struct{ IClient* mRequester; int mAfterBeat;}>

	void* onlyJesusKnows;
	// at 0xD0
	void* secrectlyProtectedUnderInternationalLawAndTheIlluminati;
};

struct CNetUDPConnetor // : INetConnector
{
	INetConnector _base;
	void* smth; // listen socket fd?
	gpg_mutex mMutex;
	// at 0x14
	SOCKET mSocket;
	// at 0x24
	linked_list mConnections;// CNetUDPConnection*
};

struct ISTIDriver
{
	void* vtable;
};
struct CSTIDriver // : ISTIDriver
{
	ISTIDriver _base;
	void* notcarenow;
	IClientManager* clientManager;
};
/** Game Types
  Multiplayer - CLobby::LaunchGame
  Replay - VCR_SetupReplaySession
  SinglePlayer - WLD_SetupSessionInfo
  Saved Game - CSavedGame::CreateSinglePlayerSession
*/

/** Session State
	0 - None?
	1 - Loading?
	2 - Started?
	3 - SIM Initialized
	4 - SIM Started
	5 - Game Started
	7 - Restart Requested
	8 - Session Halted
*/
