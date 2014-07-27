import sys
import os
from subprocess import check_output as proc_call
import shutil

if sys.version_info[0] != 3:
	print("Fuck you. Use python 3.")
	os.exit(1)

from argparse import ArgumentParser

def call(command):
	output = proc_call(command)
	if output:
		print('Executing','"%s":' % ' '.join(command))
		for line in output.splitlines():
			print(' ',line.decode())

def parseCommandLine():
	parser = ArgumentParser(description='Forged Alliance Patcher')
	parser.add_argument('-c','--c-code','--I-like-this-robust-argument-parser',
		'--although-it-is-quite-tedious',action='store_true',help='Compile the c patch version instead.')
	parser.add_argument('output_file',nargs='?',help='Optional filename to place the patched version at.')
	return parser.parse_args()

def nasm_compile(filename, filename_out = None):
	assert filename.endswith('.s')
	print("Compiling %s" % filename)

	if filename_out is None:
		filename_out = filename[:-1]+'.asm.bin'
	# Compile ext_sector.s
	call(['nasm', filename,'-o', filename_out])
	return open(filename_out,'rb').read()

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

	args = parseCommandLine()

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
	if args.c_code:
		call(['python','build_ext.py'])
		with open('build/ext_sector.bin','rb') as f:
			size = os.fstat(f.fileno()).st_size
			if size > 0x1500:
				raise RuntimeError('C compiled sector size too big.')
			else:
				pe.write( f.read() )
	else:
		pe.write( nasm_compile(ext_section_filename) )

	pe.close()
	print("Done.")
	if args.output_file:
		shutil.move(filename, args.output_file)
		print("Moved",filename,"to",args.output_file)

if __name__ == "__main__":
	main()