; HOOK LuaPlus_realloc ROffset = 0x519A90
bits 32

_HOOK_LuaPlus_realloc equ 0x128B058

org 0x919A90

mov eax, [esp+0x4]
mov eax, [eax]
mov [esp+0x4], eax

jmp _HOOK_LuaPlus_realloc

