
import datetime
import os

from common import *

funcs = [
('luaH_new',    0x926b70),
('luaH_free',   0x926bf0),
('luaH_index',  0x926e60),
('luaH_next',   0x926ef0),
('luaH_get',    0x926d60),
('luaH_getnum', 0x926ca0),
('luaH_getstr', 0x926d20),
('luaH_set',    0x926db0),
('luaH_setnum', 0x927310),
('luaV_gettable', 0x928c30),
('luaV_settable', 0x928ca0)
]

hook_jmp_offset = 0x128b028

for i in range(len(funcs)):
    name, addr = funcs[i]

    src = \
'''; HOOK %-20s ROffset = 0x%x
bits 32
org 0x%x

_HOOK_%s equ 0x%x

mov eax, 0x%x
jmp _HOOK_%s

''' % (name,
       addr - 0x400000, addr,
       name, hook_jmp_offset, i, name)

    write_hook('hook_%s.asm' % name, src)