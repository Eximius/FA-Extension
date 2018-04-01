; HOOK WinMain ROffset = 0x4D3D5D
bits 32

_HOOK_pe_init equ 0x128B040

org 0x8D3D5D

call _HOOK_pe_init
nop
