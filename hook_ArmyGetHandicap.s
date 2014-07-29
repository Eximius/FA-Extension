; HOOK ArmyGetHandicap ROffset = 0x308B0A
bits 32

_HOOK_ArmyGetHandicap equ 0x128B010

org 0x708B0A

mov eax, [esi]
push eax
call _HOOK_ArmyGetHandicap
add esp, 0x4
pop edi
add esp, 0xC
ret 

; 0x708B19 add esp, 14h
