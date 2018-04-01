
import datetime
import os

from common import *

funcs = []

for line in open('FA_asm_offsets.asm', 'rb'):
    line = line.strip().decode()
    handed = line.partition(';')[2].strip() == 'HANDED'
    line = line.partition(';')[0]
    if line:
        name, _, addr = line.split()[:3]
        funcs.append((name, int(addr, 0), len(funcs), handed))
        

with open('build/FA_emitted_hooks.lst', 'w') as f_lst:
  f_lst.write("HOOK\tROffset\tVOffset\tOrdinal\tOrdinal hex\n")
  
  for name, addr, ordinal, handed in funcs:
    f_lst.write("%s\t%x\t%x\t%d\t0x%x\n" % 
      (name, addr - 0x400000, addr, ordinal, ordinal) )

    
for name, addr, ordinal, handed in funcs:
    if handed:
      continue
    src = \
'''; HOOK %-20s ROffset = 0x%x
bits 32
org 0x%x

_HOOK_lua_proxy equ 0x128B018

mov eax, 0x%x
jmp _HOOK_lua_proxy

''' % (name,
           addr - 0x400000, addr,
           ordinal)
    
    write_hook('hook_%s.asm' % name[1:], src)
    

with open('build/FA_lua_jt.asm', 'w') as f_jt:
  f_jt.write(
''';.intel_syntax noprefix
;.arch i686
;.text

bits 32

global lua_lib_jumptable

section .text

lua_lib_jumptable:
''')

  for name, addr, ordinal, handed in funcs:
      f_jt.write('extern %s\ndd %s\n' % (name, name))

