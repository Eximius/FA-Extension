; HOOK LuaPlus_realloc ROffset = 0x50A358
bits 32

_HOOK_LuaState_Init equ 0x128B070

org 0x90A358

push ebp
push edi
push ebx
push esi
call _HOOK_LuaState_Init
add esp, 0x8
mov edi, [esi]
jmp 0x90a3ca

