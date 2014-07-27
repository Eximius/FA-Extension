import sys
import os
from subprocess import check_call as call
import shutil

from os.path import dirname, basename

if sys.version_info[0] != 3:
	print("Fuck you. Use python 3.")
	os.exit(1)

if '-v' in sys.argv:
	origCall = call
	def call(penis):
		print('Calling:',penis)
		origCall(penis)

def nasm_compile(filename, filename_out, defines = {}):
	assert filename.endswith('.s')
	print("Assembling %s" % filename)

	command = ['nasm','-felf', filename,'-o', filename_out]#+'.tmp']

	command.extend(['-d%s=%s' % (x,str(y)) for x,y in defines.items()])
	call(command)

def gcc_compile(filename, filename_out, defines = {}):
	print('Compiling %s' % filename)
	cxxflags = ['-O0','-fno-asynchronous-unwind-tables']
	for k,v in defines.items():
		cxxflags += ['-D','%s=%s' % (k,str(v))]

	call(['g++','-I.','-m32']+ cxxflags +['-c', filename,'-o', filename_out])

def gcc_link(filenames, filename_out):
	print('Linking...')
	ldflags = []
	#call(['g++','-shared','-static',
	#	  '-Wl,-T,linker.ld,-m,i386pe'] + ldflags \
	#	  + list(filenames) + ['-o',filename_out])
	call(['ld','-T','linker.ld','-static','-m','i386pe'] + ldflags \
		  + list(filenames) + ['-o',filename_out])

def rip_out_binary(filename, filename_out):
	print('Ripping out the binary from the elf...')
	call(['objcopy','-O','binary','-R','.eh_fram', filename, filename_out])

def build_ext(defines = {}):
	if not os.path.exists('build/'):
		os.makedirs('build/')

	defines.update({'FORGED_ALLIANCE':1,
					'CXX_BUILD':1})
	src = ['ext_sector.s','ext_sector_foo.cpp']
	obj = []
	for src_file in src:
		obj_file = 'build/'+basename( src_file[:src_file.rfind('.')]+'.o' )
		
		if src_file.endswith('.s'):
			nasm_compile(src_file, obj_file, defines)
		else:
			gcc_compile(src_file, obj_file, defines)
		obj += [obj_file]
	gcc_link(obj, 'build/ext_sector.exe.tmp')
	rip_out_binary('build/ext_sector.exe.tmp','build/ext_sector.bin')
	print('Done.')

if __name__ == '__main__':
	build_ext()	