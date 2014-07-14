; HOOK LoadSavedGame ROffset = 0x480CFD
bits 32

_ext_main equ 0x128B000

org 0x880CFD

mov eax, [esi]
jmp _ext_main