import sys
import os
from subprocess import check_call as call
import shutil

from os.path import dirname, basename

if os.name == 'nt': 
    gcc = "gcc"
    ld = "ld"
else:
    gcc = "i686-w64-mingw32-g++"
    ld = "i686-w64-mingw32-ld"

if sys.version_info[0] != 3:
    print("Fuck you. Use python 3.")
    os.exit(1)

if '-v' in sys.argv:
    origCall = call
    def call(penis):
        print('Calling:',penis)
        origCall(penis)

def nasm_compile(filename, filename_out, defines = {}):
    assert filename.endswith('.s') or filename.endswith('.asm')
    print("Assembling %s" % filename)

    command = ['nasm','-felf', filename,'-o', filename_out]#+'.tmp']

    command.extend(['-d%s=%s' % (x,str(y)) for x,y in defines.items()])
    call(command)

def gcc_compile(filename, filename_out, defines = {}):
    print('Compiling %s' % filename)
    cxxflags = ['-O0','-fno-exceptions','-fno-asynchronous-unwind-tables','-fno-pie', '-ffreestanding','-nostartfiles','-nostdlib','-static' ]
    
    # On linux, c code by default does not use leading underscore.
    if os.name != 'nt': 
        cxxflags += ['-fleading-underscore']

    for k,v in defines.items():
        cxxflags += ['-D','%s=%s' % (k,str(v))]
        
    call([gcc,'-I.','-I','src/','-I','../gpg-luajit/src/','-m32']+ cxxflags +['-c', filename,'-o', filename_out])

def gcc_link(filenames, filename_out):
    print('Linking...')
    ldflags = []
    #call(['g++','-shared','-static',
    #	  '-Wl,-T,linker.ld,-m,i386pe'] + ldflags \
    #	  + list(filenames) + ['-o',filename_out])
    obj_type = 'i386pe'# if os.name == 'nt' else 'elf_i386'
    call([ld,'-v','-T','linker.ld','--warn-section-align',
        '-Map','build/ext_sector.map',
        '--discard-none',
        '-nostdlib','-nostartfiles','-static',
        '-m', obj_type,
        '--file-alignment', '1024',
        '--section-alignment', '1024','-n'
        ] + ldflags \
        + ['-('] + list(filenames) + ['-)','-o',filename_out])

def rip_out_binary(filename, filename_exe_out, filename_data_out, filename_debug_out):
    print('Ripping out the binary from the elf...')
    #call(['objcopy','-O','binary','-j','.ext', filename, filename_exe_out])
    #exe = open(filename_exe_out,'rb').read()
    #open(filename_exe_out,'wb').write(exe[0x1F0:])
    #call(['objcopy','-O','binary','-j','.ext_data', filename, filename_data_out])
    #call(['objcopy','-O','binary','-j','.debug', filename, filename_debug_out])
    call(['objcopy',
        '--dump-section', '.ext=%s' % filename_exe_out, 
        '--dump-section', '.ext_data=%s' % filename_data_out, 
        '--dump-section', '.debug=%s' % filename_debug_out, 
        filename])
    call(['objcopy',
        '--strip-all',
        filename, 'build/ext_sector.lean.exe'])
    

def build_ext(defines = {}):
    if not os.path.exists('build/'):
        os.makedirs('build/')

    defines.update({'FORGED_ALLIANCE':1,
                    'CXX_BUILD':1})
    src = [] + \
    ['src/'+x for x in ['ext_sector.asm','ext_sector_foo.cpp', 'luajit_glue.cpp', 'LuaObject.cpp', 'FA_luajit_glue.asm']] + \
    ['build/'+x for x in ['FA_lua_jt.asm', 'msvc_imports.asm']]

    obj = []#['build/FA_lua_jt.o']
        #, '/usr/lib32/libc.a','/usr/lib32/crt1.o','/usr/lib32/crti.o', '/usr/lib32/crtn.o','/usr/lib32/libm.a','/usr/lib32/libstdc++.a'
        
    for src_file in src:
        obj_file = 'build/'+basename( src_file[:src_file.rfind('.')]+'.o' )
        
        if src_file.endswith('.s') or src_file.endswith('.asm'):
            nasm_compile(src_file, obj_file, defines)
        else:
            gcc_compile(src_file, obj_file, defines)
        obj += [obj_file]
    obj += ['build/libluajit.a'] + \
        ['/usr/i686-w64-mingw32/lib/'+x for x in ['libstdc++.a', 'libmingw32.a']] + \
        ['/usr/lib/gcc/i686-w64-mingw32/7.3.0/libgcc.a']
    #, 'build/libctiny.a'] + \
    # Practically, we link to these libs, just in an arcane way.
    #+ ['/usr/i686-w64-mingw32/lib/'+x for x in ['libkernel32.a', 'libmingw32.a', 'libmsvcr80.a']] + 
    
    gcc_link(obj, 'build/ext_sector.exe')
    rip_out_binary('build/ext_sector.exe','build/ext_sector_exe.bin', 'build/ext_sector_data.bin', 'build/ext_sector_debug.bin')
    print('Done.')

if __name__ == '__main__':
    build_ext()	
