
;.intel_syntax noprefix
;.arch i686
;.text

bits 32
;extern ?CastState@LuaState@LuaPlus@@SAPAV12@PAUlua_State@@@Z
;extern ??0LuaObject@LuaPlus@@QAE@ABVLuaStackObject@1@@Z
;extern ??1LuaObject@LuaPlus@@QAE@XZ
;extern ?lua_tostring@@YAPBDPAUlua_State@@H@Z
;extern ?lua_tonumber@@YAMPAUlua_State@@H@Z
;extern ?AssignClientIndex@CLobby@Moho@@AAEXHVStrArg@gpg@@AAIAAH@Z
;extern ?AssignCommandSource@CLobby@Moho@@AAEIHHAAV?$vector@USSTICommandSource@Moho@@V?$allocator@USSTICommandSource@Moho@@@std@@@std@@AAI@
;extern lua_cast_CLobby?

; MohoEngine.dll addresses
;?CastState@LuaState@LuaPlus@@SAPAV12@PAUlua_State@@@Z equ 0x105CE4B0
;??0LuaObject@LuaPlus@@QAE@ABVLuaStackObject@1@@Z equ 0x105CE280
;??1LuaObject@LuaPlus@@QAE@XZ equ 0x105CE480
;?lua_tostring@@YAPBDPAUlua_State@@H@Z equ 0x105CE290
;?lua_tonumber@@YAMPAUlua_State@@H@Z
;__imp_atoi equ 0x105CE7C4
;?AssignClientIndex@CLobby@Moho@@AAEXHVStrArg@gpg@@AAIAAH@Z equ 0x1038D150
;?AssignCommandSource@CLobby@Moho@@AAEIHHAAV?$vector@USSTICommandSource@Moho@@V?$allocator@USSTICommandSource@Moho@@@std@@@std@@AAI@ equ 0x1038D240
;lua_cast_CLobby? equ 0x103937A0

; ForgedAlliance.exe addresses
	; LuaPlus
	LuaState__CastState equ 0x90A510 ; LuaState* (lua_state*)
	LuaState__Error equ 0x90C1D0 ; (LuaState*, fmt, args...)
	?CastState@LuaState@LuaPlus@@SAPAV12@PAUlua_State@@@Z equ 0x90A510
	??0LuaObject@LuaPlus@@QAE@ABVLuaStackObject@1@@Z equ 0x908A70
	??1LuaObject@LuaPlus@@QAE@XZ equ 0x9075D0
	LuaObject_GetString equ 0x907A90

	lua_pushnumber equ 0x90CD40 ; (lua_state*, float)
	lua_pushbool equ 0x90CF80; (lua_state*, bool)

	?AssignClientIndex@CLobby@Moho@@AAEXHVStrArg@gpg@@AAIAAH@Z equ 0x7C4E80
	?AssignCommandSource@CLobby@Moho@@AAEIHHAAV?$vector@USSTICommandSource@Moho@@V?$allocator@USSTICommandSource@Moho@@@std@@@std@@AAI@ equ 0x7C4F60

	lua_cast_CLobby? equ 0x7CB7E0

	Moho__NET_MakeNATTraversal? equ 0x7BC5F0 ; (LuaObject* rvo, LuaState*)
	LuaObject_PushStack equ 0x907D80 ; (LuaStackObject* rvo, LuaState*)

	; Returned object is probably an iterator of sorts?
	; It is 0x10 bytes. 
	Moho__BVSet_Add equ 0x6E5660 ; void* __stdcall (void* this<ecx>, void* rvo<ebx>, int setItem)
	Moho__Set__Add equ 0x4036A0 ; void *__stdcall (void *this<edi>, void *rvo, int setItem<esi>)


	; Utils
	?lua_tostring@@YAPBDPAUlua_State@@H@Z equ 0x90CA90
	_lua_tostring equ 0x90CA90
	_lua_tonumber equ 0x90C9F0 
	__imp_atoi equ 0xA835CE

	__ftol2_sse equ 0xA89CC0

	lua_gettop equ 0x90C590 ; (lua_state*)

	_stricmp equ 0xAA549E ; int (const char*, const char*)

	Logf equ 0x937CB0

	; Globals

	g_STIDriver equ 0x10C4F50
	g_SWldSessionInfo equ 0x10C4F58
	g_CWldSession equ 0x10A6470

	; c Symbols

	extern _print_hello_world

; Begin the Code
%ifndef CXX_BUILD
org 0x128B000
%endif
section .ext

global _ext_lua_LoadSavedGameHook
_ext_lua_LoadSavedGameHook: ; (lua_state*<eax>)
	push ebp
	mov ebp, esp

	; Function body
	push ebx
	push ecx
	push edx
	push esi

	mov edx, eax ; edx = lua_state*

	mov eax, esp
	call _print_int

	; Get int function id (first param)
		push 1
		push edx

		call _lua_toint
		add esp, 0x8

	cmp eax, 0x2
	jnb .unknown_func

		push edx

		; Call the func
		mov ebx, _function_table

		mov ebx, [ebx+eax*4]

		call ebx

		add esp, 0x4

		jmp .cleanup
	.unknown_func:


	; Function end
	.cleanup:
	mov ecx, eax
	mov eax, esp
	call _print_int
	mov eax, ecx

	pop esi
	pop edx
	pop ecx
	pop ebx
	pop ebp
	; Balance stack in lua_LoadSavedGame
	mov ecx, [ebp-0xC] ; Clear SEH
	mov [fs:0], ecx

	pop edi
	pop esi
	pop ebx
	
	mov esp, ebp
	pop ebp
	ret
align 0x4

aExtD: db 'EXT: %08x',0
align 0x4

aExtS: db 'EXT: %s',0
align 0x4

aEXTError: db 'EXTError',0
align 0x4

_print_int: ; (int<eax>)
	push ecx
	push edx

	push eax
	push aExtD
	call Logf
	add esp, 0x8

	pop edx
	pop ecx
	ret

align 0x4
_print_str: ; (const char*<eax>)
	push ecx
	push edx

	push eax
	push aExtS
	call Logf
	add esp, 0x8

	pop edx
	pop ecx
	ret

align 0x4
global _Logf
_Logf:
	jmp Logf
	
align 0x4
_lua_toint: ; (lus_state*, int)
	; LuaPlus lua_tostring does not work for integers/numbers
	; found it out the really hard way
	; So here's a new new new one 
	push ebp
	mov ebp, esp

	push ecx
	push edx

	mov eax, [ebp+0xc]
	push eax
	mov eax, [ebp+0x8]
	push eax

	call _lua_tonumber
	call __ftol2_sse

	add esp, 0x8
	pop edx
	pop ecx

	pop ebp
	ret

align 0x4
_lua_exit:; (lua_state*<eax>)
	push eax
	call LuaState__CastState
	push aEXTError
	push eax
	call LuaState__Error
	ret

align 0x4
_moho_set_hackadd: ; (void* this<eax>, int setItem<ecx>)
; Adds the item to the set without checking for any size boundaries
	mov eax, [eax+0x8]
	mov edx, ecx
	shr edx, 5
	and ecx, 0x1f 
	lea eax, [eax+edx*4]
	mov edx, 1
	shl edx, cl
	or [eax], edx
	ret
align 0x4
_string_data: ; const void* (string* this<ecx>)
	cmp dword [ecx+0x18], 0x10
	jb .sso
		mov eax, [ecx+0x4]
		ret
	.sso:
		lea eax, [ecx+0x4]
		ret
align 0x4

_function_table:
	dd _info
	dd _add_validCmdSource

_ext_version: dd 0.01

align 0x4
_info: ; (lua_state*)
; Returns version for this extension
	
	call _print_hello_world

	mov eax, [_ext_version]
	push eax
	mov eax, [esp+0x8]
	push eax
	call lua_pushnumber
	add esp, 0x8

	mov eax, 1 ; Returns 1 result to lua
	ret
align 0x4
_add_validCmdSource: ; (lua_state*)
; Adds valid command source for a player
; Lua args: (playerName, sourceId)
	push ebp
	mov ebp, esp

	push ebx
	push ecx
	push edx
	push esi
	push edi

	mov edx, [ebp+0x8] ; edx = lua_state*

	; Get ClientManager*
	mov ebx, [g_STIDriver]
	mov ebx, [ebx+0x8] ; ebx = IClientManager*

	; Get Client*
	push 2
	push edx
	call _lua_tostring

	add esp, 0x8

	mov esi, eax

	mov edi, [ebx+0x420]
	mov ebx, [ebx+0x424]

	.it_loop:
		cmp edi, ebx
		je .it_end

		mov ecx, [edi]
		add ecx, 0x4
		call _string_data

		push esi
		push eax
		call _stricmp
		add esp, 0x8

		test eax, eax

		jz .found

		add edi, 0x4
		jmp .it_loop
	.it_end: ; User not found
		; Handle invalid user.
		jmp .locret
	.found:
		mov ecx, [edi] ; ecx = Client*

	mov edx, [ebp+0x8] ; edx = lua_state*
	; Get sourceId
		push 3
		push edx
		call _lua_toint
		add esp, 0x8

	; Set Client* to have the valid source
	mov ebx, eax
	lea eax, [ecx+0x30]
	mov ecx, ebx

	call _moho_set_hackadd

	; Set UserArmy* to have the valid source
	mov eax, [g_CWldSession]
	mov eax, [eax+0x3F0]
	mov eax, [eax+ebx*4]
	add eax, 0x128

	; Tiny fix to seemingly bugged set
	mov edx, [eax+0x8]
	add edx, 4
	mov [eax+0xc], edx

	mov ecx, ebx

	call _moho_set_hackadd
	;sub esp, 0x10

	;add ecx, 0x30
	;mov ebx, esp
	;push eax
	;call _print_int
	;call Moho__BVSet_Add

	;mov esi, eax
	;call _print_int
	;lea edi, [ecx+0x38]
	;push esp
	;call Moho__Set__Add
	
	;add esp, 0x10

	mov eax, 0 ; Returns 0 results to lua
.locret:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx

	pop ebp
	ret

align 0x4
_make_nat: ; (lua_state*)
; Makes a NatTraversalProvider and returns it to lua
; Does not work.... yet...??..
	push ecx

	mov ecx, [esp+0xc] ; ecx = lua_state*
	push ecx
	call ?CastState@LuaState@LuaPlus@@SAPAV12@PAUlua_State@@@Z ; eax = casted LuaState
	add esp, 0x4

	push eax
	lea eax, [esp-0x100]
	push eax
	call Moho__NET_MakeNATTraversal?
	add esp, 0x8

	push ecx
	lea ecx, [esp-0x200]
	push ecx ; rvo for LuaStackObject
	lea ecx, [esp-0x100] ; ecx = LuaObject*
	call LuaObject_PushStack

	pop ecx
	ret

_ext_Clobby_addCOmmandSOurce: ; Broken code, do not use, here for reference
	push edx
	call ?CastState@LuaState@LuaPlus@@SAPAV12@PAUlua_State@@@Z ; eax = casted LuaState
	add esp, 0x4

	; Get CLobby instance

	  	; Create LuaStackObject
		mov     [ebp - 0x8], eax
		mov     [ebp - 0x4], dword 1

		; push LuaStackObject Address
		lea     ecx, [ebp - 0x8]
		push ecx 

		; LuaObject(const LuaStackObject&)
		lea     ecx, [ebp - 0x1c]
		call    ??0LuaObject@LuaPlus@@QAE@ABVLuaStackObject@1@@Z

		lea     eax, [ebp - 0x4]
		call    lua_cast_CLobby?
		lea     ecx, [ebp - 0x4]
		mov     ecx, eax ; ecx = CLobby*
		call    ??1LuaObject@LuaPlus@@QAE@XZ

	; Get PlayerName
	push -1
	push edx

	call ?lua_tostring@@YAPBDPAUlua_State@@H@Z ; eax = playerName
	add esp, 0x8

	mov ebx, eax ; ebx = playerName

	; Get OwnerID
	push -2
	push edx

	call _lua_toint
	add esp, 0x8

	mov esi, eax ; esi = OwnerID

	; Do the thing
	mov edx, [ebp - 0x100]

	push edx
	push edx
	push ebx
	push eax

	call ?AssignClientIndex@CLobby@Moho@@AAEXHVStrArg@gpg@@AAIAAH@Z

	push edx
	push edx
	push 0
	push esi

	call ?AssignCommandSource@CLobby@Moho@@AAEIHHAAV?$vector@USSTICommandSource@Moho@@V?$allocator@USSTICommandSource@Moho@@@std@@@std@@AAI@
