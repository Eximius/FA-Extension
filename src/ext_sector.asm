
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

    ;	struct LuaStackObject
    ;	{
    ;		// 0x8 bytes
    ;		LuaState* state;
    ;		int stack_index;
    ;	};
    
    LuaStackObject__GetBoolean equ 0x415560

    lua_pushnumber equ 0x90CD40 ; (lua_state*, float)
    _lua_pushbool equ 0x90CF80; (lua_state*, bool)

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
    _lua_toboolean equ 0x90CA40
    __imp_atoi equ 0xA835CE

    __ftol2_sse equ 0xA89CC0

    lua_gettop equ 0x90C590 ; (lua_state*)

    _stricmp equ 0xAA549E ; int (const char*, const char*)

    Logf equ 0x937CB0
    _Logf equ 0x937CB0

    ; Globals

    g_STIDriver equ 0x10C4F50
    g_SWldSessionInfo equ 0x10C4F58
    g_CWldSession equ 0x10A6470
    _g_CWldSession equ 0x10A6470

    _g_Sim equ 0x10A63F0

    ; c Symbols

        ; Imports
        extern _print_hello_world
        extern _ext_ValidateFocusArmyRequest
        extern _cxx_AddCommandSourceId
        extern _cxx_SetCommandSourceId
        extern _cxx_lua_library_call
        
        extern lua_lib_jumptable

        extern __imp__printf
        _printf equ __imp__printf
                
        ; Exports
        global _stricmp

        global _lua_pushbool

        global _Logf

        global _g_CWldSession
        global _g_Sim
        
        global _print_str

        
        extern __pei386_runtime_relocator
        global _pei386_runtime_relocator
        
        _pei386_runtime_relocator equ __pei386_runtime_relocator
        
        global _gpg_meta_io
        _gpg_meta_io equ 0xD44C20

        global _gpg_open_io_and_os
        _gpg_open_io_and_os equ 0x917410

        global __ZN4moho9LuaObject15__AddToUsedListEPNS_8LuaStateE
        __ZN4moho9LuaObject15__AddToUsedListEPNS_8LuaStateE equ 0x9080f0

        global _moho_throw_lua_error_traceback
        _moho_throw_lua_error_traceback equ 0x912A10
        
        global _gpg_InitSoundSystem 
        _gpg_InitSoundSystem equ 0x4D88C0


        global _RType_new_defcon
        _RType_new_defcon equ 0x924260

        global _RType_new_copycon
        _RType_new_copycon equ 0x924340
        
section .rdata

s_fucked db `Welp, that's a bad api ordinal.\n`,0
s_fucked_env db `Welp, somebody wants to push the env.\n`,0
s_lua_call db `lua library call 0x%x ( 0x%08x, 0x%08x, 0x%08x, 0x%08x) from 0x%x\n`,0
s_found_luajit db `Found luajit state 0x%x\n`,0

; Begin the Code
%ifndef CXX_BUILD
org 0x128B000
%endif
section .ext

; <Area for hooks>
_HOOK_lua_LoadSavedGame: ; (lua_state*<eax>)
    jmp _ext_do_shit_tm

align 0x8
_HOOK_CWldSession__ValidateFocusArmyRequest: ; (int)
    jmp _ext_ValidateFocusArmyRequest

align 0x8
_HOOK_ArmyGetHandicap: ; (lua_state*)
    jmp _ArmyGetHandicap_addValidCmdSource
; </ Area for hooks>

extern _check_load_msvc_imports
extern eat_shit_and_die

align 0x8
_HOOK_lua_library_call:
    jmp __lua_library_call

extern __imp__realloc
_realloc equ __imp__realloc

align 0x8
_HOOK_odd_mem_alloc:
    jmp eat_shit_and_die
;    jmp _realloc

; At 0x28
align 0x8
extern _log_luaH
_HOOK_luaH_xxx:
    push eax
    call _log_luaH
    pop eax
    mov ecx, [esp+0x4]
    mov ecx, [ecx]
    mov [esp+0x4], ecx
    jmp [luaH_jumptable + eax*4]

; At 0x40
align 0x20
_HOOK_pe_init:
    sub esp, 0x12C ; Part of hooked function
    call _check_load_msvc_imports
    
    jmp [esp + 0x12C] ; Funky return

extern _glue_lj_alloc

; At 0x58
align 0x8
_HOOK_lj_alloc_f: ; (lua_State* L, void* ptr, int osize, int nsize)
    jmp _glue_lj_alloc

align 0x8
_HOOK_moho_f:
    mov eax, [moho_table + eax*4]
    jmp eax

; At 0x70
align 0x8
extern _LuaState_Init
_HOOK_LuaState_Init:
    ; mov eax, [esp+0x4]
    ; mov eax, [eax]
    ; mov [esp+0x4], eax
    jmp _LuaState_Init

; At 0x80
align 0x10
extern _LuaObject_Register
_HOOK_LuaObject_Register:
    jmp _LuaObject_Register

align 0x10
extern _luaS_newlstr
_HOOK_luaS_newlstr
    jmp _luaS_newlstr

; At 0x100
align 0x10
extern _luaplus_getstateuserdata ; void* (lua_State* L)
_HOOK_lua_getstateuserdata
    jmp _luaplus_getstateuserdata
; align 0x10
; extern _LuaState_GetGlobals
; _HOOK_LuaState_GetGlobals
;     jmp _LuaState_GetGlobals

align 0x4

moho_table:
dd 0
dd 0
dd 0
extern _lua_newuserdata_RTypep
dd _lua_newuserdata_RTypep


align 0x4

luaH_jumptable:

extern _luaH_new
dd _luaH_new

extern _luaH_free
dd _luaH_free

extern _luaH_index
dd _luaH_index

extern _luaH_next
dd _luaH_next

extern _luaH_get
dd _luaH_get

extern _luaH_getnum
dd _luaH_getnum

extern _luaH_getstr
dd _luaH_getstr

extern _luaH_set
dd _luaH_set

extern _luaH_setnum
dd _luaH_setnum

extern _luaV_gettable
dd _luaV_gettable

extern _luaV_settable
dd _luaV_settable

; extern _lj_tab_new
; dd luaH_new

; extern _lj_tab_free
; dd _lj_tab_free

; extern _keyindex
; dd _keyindex

; extern _lj_tab_next
; dd _lj_tab_next

; extern _lj_tab_get
; dd _lj_tab_get

; extern _lj_tab_getinth
; dd _lj_tab_getinth

; extern _lj_tab_getstr
; dd _lj_tab_getstr

; extern _lj_tab_set
; dd _lj_tab_set

; extern _lj_tab_setinth
; dd _lj_tab_setinth

; extern _luaV_gettable
; dd _luaV_gettable

; extern _luaV_settable
; dd _luaV_settable


extern _proxy_malloc

; align 0x10
; luaH_new:
;     ; Luajit is not abi compatible in this one
;     push dword [esp+0xC] 
;     push dword [esp+0xC]
;     ;add dword [esp], 1
;     push dword [esp+0xC]
;     call _lj_tab_new
;     push ebx
;     mov ebx, eax

;     push 0x8
;     call _proxy_malloc
;     mov dword [eax+4], 5
;     mov [eax], ebx

;     add esp, 0x4
;     pop ebx
;     add esp, 0xC
;     ret


; Other guff

extern _log_lua_call
 
align 0x4
__lua_library_call: ; (int lua_api_ordinal<eax>)
    ;push ebx
    ;call _check_load_msvc_imports
    push dword [esp]
    push dword [esp+0x14]
    push dword [esp+0x14]
    push dword [esp+0x14]
    push dword [esp+0x14]

    push eax
    ; push s_lua_call
    ; call _printf
    ; add esp, 0x4
    call _log_lua_call
    pop eax

    add esp, 0x14

    mov ecx, eax ; Temp save ordinal for logging in glue.s
    cmp eax, 0
    jz .lua_open

    cmp eax, 0x6b
    jge .fuck

    push eax
    push ecx
    call __check_luajit_state
    pop ecx
    pop eax

    ; Load the real lua_State*
    mov edx, [esp + 0x4] 
    cmp dword [edx+0x4], 0
    jnz .luajit_state
        mov edx, [edx]
        mov [esp + 0x4], edx
        jmp .cont_run
    .luajit_state:
        push eax

        push edx
        push s_found_luajit
        call _printf
        add esp, 0x8

        pop eax
    .cont_run:

    cmp eax, 5 ; lua_pushvalue
    jz .check_env

    .lua_open:
    mov eax, [cs:lua_lib_jumptable+eax*4]
    nop
    jmp eax

    align 0x10
    .fuck:
        push s_fucked
        call _printf
        call eat_shit_and_die
        ret

    .check_env:
        cmp dword [esp+4], 0xffffd8ef
        jz .fuck_env
        mov eax, [cs:lua_lib_jumptable+eax*4]
        jmp eax

    .fuck_env:
        push s_fucked_env
        call _printf
        call eat_shit_and_die
        ret
    ; .lua_open:
    ;     mov eax, [lua_lib_jumptable + eax*4]
        ; jmp eax

section .data
align 0x4
lj_state_crc32 dd 0

global _lj_state_p
_lj_state_p     dd 0

s_lua_changed db `Lua state changed in between calls\n`,0

s_ecx db `ecx: %08x, esp: %08x\n`,0
section .ext

__check_luajit_state:
    ; push esp
    ; push ecx
    ; push s_ecx
    ; call _printf
    ; add esp, 0xC
    ret

    xor eax, eax
    xor ecx, ecx

    ; push esp
    ; push ecx
    ; push s_ecx
    ; call _printf
    ; add esp, 0xC


    mov edx, [_lj_state_p]

    cmp esp, 0x15df7af 
    jz eat_shit_and_die

    ; .iter:
    ;     crc32 eax, dword [edx + ecx * 4]
    ;     inc ecx
    ;     cmp ecx, 16
    ;     jnz .iter

    cmp [lj_state_crc32], eax
    jnz .fuck
    ret
    .fuck:
        cmp dword [lj_state_crc32], 0
        jz .not_fuck

        push s_lua_changed
        call _printf
        ;call eat_shit_and_die
        ret
    .not_fuck:
        mov [lj_state_crc32], eax
        ret

align 0x4
_ArmyGetHandicap_addValidCmdSource:
    push ebx
    mov ebx, [esp+0x8] ; ebx = lua_state*

    push ebx
    call lua_gettop
    add esp, 0x4

    cmp eax, 0x3
    jnz .bad_argument_list
        ; Get set_or_unset
        push 3
        push ebx
        call _lua_toboolean
        add esp, 0x8

        push eax ; push set_or_unset

        ; Get sourceId
        push 2
        push ebx
        call _lua_toint
        add esp, 0x8

        push eax ; push sourceId

        ; Get ArmyId
        push 1
        push ebx
        call _lua_toint
        add esp, 0x8

        push eax ; push armyId
        ; Add source id to SimArmy

        push ebx ; lua_state*
        call _cxx_SetCommandSourceId
        add esp, 0x10

        pop ebx
        ret
    .bad_argument_list:
        ; Return the same error as usual.
        push    eax
        push    1
        mov     eax, [0x10B8AF4]
        push    eax
        push    0xE0A220   ; "%s\n  expected %d args, but got %d"
        push    esi
        call    LuaState__Error
        ret 
    
align 0x4
_ext_do_shit_tm: ; (lua_state*<eax>)
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

    cmp eax, 0x1
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

extern _printf

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
    ;mov edx, [eax+0x8]
    ;add edx, 4
    ;mov [eax+0xc], edx

    ;mov ecx, ebx

    ;call _moho_set_hackadd
    
    ; Add source id to SimArmy

    push ebx ; Source id
    push esi ; PlayerName

    mov eax, [ebp+0x8]
    push eax ; lua_state*
    call _cxx_AddCommandSourceId
    add esp, 0xC
    ;sub esp, 0x10

    ;add ecx, 0x30
    ;mov ebx, esp
    ;push eax
    ;call _print_int
    ;call Moho__BVSet_Add

    ;mov esi, ecx
    ;mov edi, eax
    ;push esp
    ;call Moho__Set__Add

    ;add esp, 0x10

    ;mov eax, 0 ; Returns 0 results to lua
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
    ret