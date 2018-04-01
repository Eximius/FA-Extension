;.intel_syntax noprefix
;.arch i686
;.text

bits 32

; This file is needed to define the lua api calls which in luajit are just defines

section .rdata

sh_str db `Shitdicks\n`,0
s_info db `Unimplemented lua api ordinal: 0x%x %d\n`,0
s_MSVC_Loaded db `MSVC loaded!\n`, 0

; Imports
extern _luaL_newstate
extern _print_str
;extern _memcpy
;extern _printf
extern __imp__printf
_printf equ __imp__printf

extern _lua_createtable
extern _lua_getfield

; Exports
global _lua_open
global _lua_tostring
global _lua_strlen
global _lua_newtable
global _lua_getgcthreshold
global _lua_getgccount
global _lua_setgcthreshold
global _lua_version
global _lua_pushupvalues
global _luaL_getmetatable
global _luaL_findstring
global _luaL_getn
global _luaL_setn
global _lua_dofile
global _lua_dostring
global _lua_dobuffer

section .data
; Variables
msvc_loaded dd 0

; Crap for debug symbols to work for FA.exe
; Do not ask.

section .dummy_0
dd 0
section .dummy_1
dd 0
section .dummy_2
dd 0
section .dummy_3
dd 0
section .dummy_4
dd 0
section .dummy_5
dd 0

; Code

section .text

extern _gpg_luaL_newstate

_lua_open:
    ;jmp _luaL_newstate
    jmp _gpg_luaL_newstate

; #define lua_tostring(L,i)       lua_tolstring(L, (i), NULL)
;_lua_tostring:
;    push 0
;    push [esp+8]
;    push [esp+16]
;    call _lua_tolstring
;    mov esp, esp+12
;    ret
    
;_lua_strlen:
;    jmp _lua_objlen
    
_lua_newtable:
    push 0
    push 0
    push dword [esp+12]
    call _lua_createtable
    add esp, 0xC
    ret
    
_luaL_getmetatable: ; (lua_State *L, const char *tname);
; #define luaL_getmetatable(L,n)  (lua_getfield(L, LUA_REGISTRYINDEX, (n)))
    mov eax, [esp + 0x4]
    mov ecx, [esp + 0x8]
    push ecx
    push dword -10000
    push eax
    call _lua_getfield
    add esp, 0xC
    ret

extern _lua_tolstring

_lua_tostring:
    push 0
    push dword [esp + 0xC]
    push dword [esp + 0xC]
    call _lua_tolstring
    add esp, 0xC
    ret
    
_lua_strlen:
;_lua_newtable:
_lua_getgcthreshold:    
_lua_getgccount:
_lua_setgcthreshold:
_lua_version:
_lua_pushupvalues:
_luaL_findstring:
;_luaL_getn:
_luaL_setn:
_lua_dofile:
_lua_dostring:
_lua_dobuffer:
    push ecx
    push ecx
    push s_info
    call _printf
    call eat_shit_and_die
    ret

align 0x4

extern _lua_pcall

global _glue_lua_pcall
_glue_lua_pcall:
; lua_pcall in FA is (L, int, int) instead of lua 5.0 (L, int, int, int errfunc)
push 0
push dword [esp+0x10]
push dword [esp+0x10]
push dword [esp+0x10]
call _lua_pcall
add esp, 0x10
ret

align 0x4

_memcpy: ; (void*, const void*, int)
    mov eax, [esp+0x8] ; src
    mov ecx, [esp+0x4] ; dest
    mov edx, [esp+0xc] ; size
    
    push ebx
    .loop:
        mov ebx, [eax]
        mov [ecx], ebx
        inc ecx
        inc eax
        
        dec edx
        test edx, edx
        jnz .loop
    
    pop ebx
    ret



; Symbols required from kernel32.dll and msvcr80.dll

global _eat_shit_and_die
global eat_shit_and_die
_eat_shit_and_die:
eat_shit_and_die:
    push sh_str
    call _printf
    mov eax, [0]
    ret

extern __imp___errno
local_msvc_import_table equ __imp___errno


global _check_load_msvc_imports
_check_load_msvc_imports:
    push eax
    mov eax, [msvc_loaded]
    test eax, eax
    jz .init
    pop eax
    ret
    .init:
        ; push 0x84
        ; push msvcr80_imports
        ; push local_msvc_import_table
        ; call _memcpy
        ; FIXME! Horribly unclear why this doesn't happen naturally.
        ; extern _gpg_InitSoundSystem
        ; call _gpg_InitSoundSystem
        push ecx
        push edx
        mov ecx, local_msvc_import_table
        mov eax, 2
        .set_var:
            mov edx, [ecx]
            ; mov edx, [edx]
            mov [ecx], edx
            add ecx, 4
            dec eax
            test eax, eax
            jnz .set_var
        
        mov [msvc_loaded], dword 1
        
        push s_MSVC_Loaded

        mov eax, __imp__printf
        
        call eax
        ; add esp, 0x10
        add esp, 0x4
        pop edx
        pop ecx
        pop eax
        ret 

align 0x4
global _moho_throw_lua_error
_moho_throw_lua_error: ; (lua_State* L)
    mov esi, [esp+0x4]
    sub esp, 0x38
    jmp 0x912AA6