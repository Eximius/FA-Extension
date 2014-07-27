/**
  MohoEngine.dll Disassembling notes
*/

// Nothing here is concrete.

typedef unsigned int uint;

struct string
{
	// 0x1c bytes
	void* ptr1;
	void* data; // SSO space start
	void* ptr3;
	void* ptr4;
	void* ptr5;
	void* ptr6;
	// at 0x18
	int size; // size < 0x10 => SSO
};

struct vector
{
	void* unknown1;
	void* objects_begin;
	void* objects_end;
	void* objects_capacity_end;
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
struct LuaState
{
	lua_state* _lua_state;
	void* fuckknows1;
	void* knowsfuck2;
	void* fnows3kuck;
	void* important1;

};
struct LuaStackObject
{
	// 0x8 bytes
	LuaState* state;
	int stack_index;
};
struct LuaObject
{
	// 0x14 bytes

	void* unknown1; // objects_end?
	void* unknown2; // objects_start?
	LuaState* m_state;
	int type;
	void* unknown5; // n_objects??

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
	uint* memory;
	uint* memory_end;
	void* unknown5;
	void* unknown6;
	void* unknown7;
	void* unknown8;

#ifdef CXX_BUILD
	void add(int item)
	{
		memory[item>>5] |= 1 << (item & 0x1f);
	}

	bool operator[](int item)
	{
		return memory[item>>5] & (1 << (item & 0x1f));
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

	#ifndef FORGED_ALLIANCE
	char datas[0x130];
	#else
	char datas[0x128];
	#endif
	// at 0x130 Moho | at 0x128 FA
	moho_set mValidCommandSources;
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

	char stuff[0x3d0];

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
