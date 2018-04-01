; HOOK ValidateFocusArmy ROffset = 0x496691
bits 32

_HOOK_CWldSession__ValidateFocusArmyRequest equ 0x128B008

org 0x896691

push eax
call _HOOK_CWldSession__ValidateFocusArmyRequest
add esp, 0x4
test eax, eax ; at 0x896697
align 0x10
nop
align 0x8
; 0x8966A9 jnz loc_89667f
