; HOOK LuaPlus_realloc ROffset = 0x524130
bits 32

_HOOK_luaS_newlstr equ 0x128B090

org 0x924130

mov eax, [esp+0x4]
mov eax, [eax]
mov [esp+0x4], eax

jmp _HOOK_luaS_newlstr

