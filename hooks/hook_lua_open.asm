; HOOK lua_open ROffset = 0x523F20
bits 32

_HOOK_lua_library_call equ 0x128B018

org 0x923F20

mov eax, 0
call _HOOK_lua_library_call 
ret

; Old continue code
; push ebp
; mov esp, ebp
; push ebx
; push esi
; push edi
; mov eax, esi
; xor ebx, ebx
; mov byte [esi+0x4], 9
; mov [esi+0x5], bl
; jmp    0x923f98

