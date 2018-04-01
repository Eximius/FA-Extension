
import datetime
import os, sys

from common import *

addr_map = {}

with open('build/ext_sector.map', 'r') as mapf:
	_read_state = 0
	while True:
		line = mapf.readline()
		if _read_state == 0 and line.strip() == 'Linker script and memory map':
			_read_state = 1
		elif _read_state == 1 and line.strip().startswith('.text'):
			break

	sect = '!unk'
	while True:
		if line.strip().startswith('.rdata'):
			break
		parts = line.split()
		while len(parts) > 1:

			addr, size, name = '','',''
			p_i = 0
			try:
				addr = int(parts[p_i],0)
				p_i = 1
			except ValueError:
				sect = parts[0]
				addr = int(parts[1], 0)
				p_i = 2

			if sect != '*fill*':
				try:
					size = int(parts[p_i],0)
					if p_i+1 >= len(parts):
						print("Section %s : 0x%x 0x%x" % (sect, addr, size))
						break
					name = parts[p_i+1]
				except ValueError:
					size = -1
					name = parts[p_i]

				if size != -1:
					break # Only object file addresses

				# Finally somewhere useful

				# print("%s 0x%x %s" % (sect, addr, name))

				addr_map[name] = addr

			break # fake loop
		line = mapf.readline()


with open('hooks/_map_hooks.lst', 'r') as hook_map:
	for line in hook_map:
		if line[0] == ';' or line == '\n':
			continue

		line = line.strip().split()

		addr, op, target = line[:3]
		addr = int(addr, 0)
		name = target

		try:
			target_addr = addr_map[target[1:]]
		except KeyError:
			print("Failed to resolve address `%s`" % target)
			sys.exit(-1)

		if op == 'jmp':
			src = \
	'''; HOOK %-20s ROffset = 0x%x
	bits 32
	org 0x%x

	_HOOK_%s equ 0x%x

	jmp _HOOK_%s

	''' % (name,
			addr - 0x400000, addr,
			target, target_addr, target)
		elif op == 'this_jmp':
			size_args = int(line[3], 0)

			s_push_args = (size_args//4)*("push dword [esp + 0x%x]\n" % size_args)
			src = \
	'''; HOOK %-20s ROffset = 0x%x
	bits 32
	org 0x%x

	_HOOK_%s equ 0x%x

	%s
	push ecx

	call _HOOK_%s
	add esp, 0x%x

	; Assume stdcall

	retn 0x%x
	''' % (name,
			addr - 0x400000, addr,
			target, target_addr,
			s_push_args,
			target, size_args + 0x4, size_args)
		else:
			raise "Unknown op: %s" % op

		
		print("EMIT Hook %-20s 0x%x -> 0x%x" % (name, addr, target_addr))
		write_hook('hook%s.asm' % name, src)