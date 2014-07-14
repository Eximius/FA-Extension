import sys
import os
from subprocess import check_call as call
import shutil

if sys.version_info[0] != 3:
	print("Fuck you. Use python 3.")
	os.exit(1)

def nasm_compile(filename):
	assert filename.endswith('.s')
	print("Compiling %s" % filename)
	# Compile ext_sector.s
	call(['nasm', filename])
	return open(filename[:-2],'rb').read()

def apply_hook(pe, filename):
	hookf = open(filename, 'rb')
	head = hookf.readline().split()
	if head[1] != b'HOOK':
		raise Exception("apply_hook with a non-hook file: "+filename)

	print("Applying hook '"+head[2].decode()+"'")

	if head[3] != b'ROffset':
		raise Exception("No hook offset")
	raw_off = int(head[5], 0)
	hookf.close()

	code = nasm_compile(filename)

	print("Applying at 0x%08x: %s" % (raw_off, code))
	pe.seek(raw_off)
	pe.write(code)

def main():

	print("Patching ForgedAlliance_base.exe to ForgedAlliance_ext.exe")

	shutil.copyfile('ForgedAlliance_base.exe', 'ForgedAlliance_ext.exe')
	# Patch the binary
	filename = 'ForgedAlliance_ext.exe'

	pe = open(filename,'r+b')

	apply_hook(pe, 'hook_LoadSavedGame.s')

	verisign_offset = 0xBDD000
	verisign_size = 0x1500

	pe.seek(verisign_offset)
	pe.write( bytes([0]*verisign_size) )

	ext_section_offset = 0xBDD000
	ext_section_filename = 'ext_sector.s'

	pe.seek(ext_section_offset)
	pe.write( nasm_compile(ext_section_filename) )

	pe.close()
	print("Done.")
	if len(sys.argv) > 1:
		shutil.move(filename, sys.argv[1])
		print("Moved",filename,"to",sys.argv[1])

if __name__ == "__main__":
	main()