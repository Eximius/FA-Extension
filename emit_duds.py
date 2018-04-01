'''
This file duds out a few functions that should not be called anymore.
'''
import datetime
import os

from common import *

funcs = [
('_gc_traverse',    0x9155e0),
('luaD_checkstack', 0x9131f0),
('lua_setusergcfunction', 0x9238d0),

]

for i in range(len(funcs)):
  name, addr = funcs[i]

  src = \
'''; HOOK %-20s ROffset = 0x%x
bits 32
org 0x%x

ret

''' % (name,
       addr - 0x400000, addr)

  write_hook('hooks/hook_dud_%s.asm' % name, src)