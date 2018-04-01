import sys
import os
from subprocess import check_output as proc_call, CalledProcessError
import shutil
from os.path import join as pjoin

def pwrite(fd, data, offset):
    pos = os.lseek(fd, 0, os.SEEK_CUR)
    os.lseek(fd, offset, os.SEEK_SET)
    assert os.write(fd, data) == len(data)
    os.lseek(fd, pos, os.SEEK_SET)

if sys.version_info[0] != 3:
    print("Fuck you. Use python 3.")
    os.exit(1)

from argparse import ArgumentParser

def call(command, loud = True):
    error = False
    try:
        output = proc_call(command)
    except CalledProcessError as e:
        output = e.output
        error = True
    if loud:
        print('Executing','"%s":' % ' '.join(command))
    if output:
        for line in output.splitlines():
            print(' ',line.decode())
    if error:
        print("Error -> exiting...")
        sys.exit(1)

def silly_hand_coded_sector_patch(file):
    fd = file.fileno()
    # old patch with .ext and 0x1500 space
    #patch = { 0x136 : '07',
            #0x181 : 'C5',
            #0x188 : 'AC 24',
            #0x1B9 : 'F0 E3 00 EC B8 04 00',
            #0x1C8 : '00 00 00 00 00 00 00 00',
            #0x318 : '2E 65 78 74 00 00 00 00 '
                    #'00 15 00 00 00 B0 E8 00 00 15 00 00 00 D0 BD 00 '
                    #'00 00 00 00 00 00 00 00 00 00 00 00 20 00 00 60'}

    # FA 3603
    #patch = { 0x136 : '08',
            #0x181 : '20 f9',
            #0x188 : '8d 7e be',
            ##0x1B9 : 'F0 E3 00 EC B8 04 00',
            #0x1C8 : '00 00 00 00 00 00 00 00',
            #0x318 : '2E 65 78 74 00 00 00 00 '
                    #'00 00 10 00 00 b0 e8 00  00 00 10 00 00 d0 bd 00'
                    #'00 00 00 00 00 00 00 00 00 00 00 00 20 00 00 60',
            #0x340 : '2e 65 78 74 5f 64 61 74  00 50 00 00 00 d0 f8 00'
                    #'00 50 00 00 00 d0 cd 00  00 00 00 00 00 00 00 00'
                    #'00 00 00 00 c0 00 00 c0  00 00 00 00 00 00 00 00'
                    #'00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00' }
    # FA 3599
    patch = { 0x136 : '08',
            0x178 : '00 00 f9 00',
            0x180 : '8a 33 be 00',
            0x310 : '2e 65 78 74 00 00 00 00  00 00 10 00 00 b0 e8 00'
                    '00 00 10 00 00 d0 bd 00  00 00 00 00 00 00 00 00'
                    '00 00 00 00 60 00 00 60  2e 65 78 74 5f 64 61 74'
                    '00 50 00 00 00 b0 f8 00  00 50 00 00 00 d0 cd 00'
                    '00 00 00 00 00 00 00 00  00 00 00 00 c0 00 00 c0'}
    
    for pos, diff in patch.items():
        pwrite(fd, bytes.fromhex(diff), pos)

def parseCommandLine():
    parser = ArgumentParser(description='Forged Alliance Patcher')
    parser.add_argument('-c','--c-code','--I-like-this-robust-argument-parser',
        '--although-it-is-quite-tedious',action='store_true',help='Compile the c patch version instead.')
    parser.add_argument('output_file',nargs='?',help='Optional filename to place the patched version at.')
    return parser.parse_args()

def nasm_compile(filename, filename_out = None):
    assert filename.endswith('.s') or filename.endswith('.asm')
    #print("Compiling %s" % filename)

    if not os.path.exists('build/'+os.path.dirname(filename)):
        os.makedirs('build/'+os.path.dirname(filename))
        
    if filename_out is None:
        filename_out = 'build/'+filename[:-2]+'.asm.bin'
    # Compile ext_sector.s
    call(['nasm', filename,'-o', filename_out], False)
    return open(filename_out,'rb').read()

def apply_hook(pe, filename):
    hookf = open(filename, 'rb')
    head = hookf.readline().split()
    if head[1] == b'DISABLED':
        return
    if head[1] != b'HOOK':
        raise Exception("apply_hook with a non-hook file: "+filename)

    #print("Applying hook '"+head[2].decode()+"'")

    if head[3] != b'ROffset':
        raise Exception("No hook offset")
    raw_off = int(head[5], 0)
    hookf.close()

    code = nasm_compile(filename)

    #print("Apply at 0x%08x 0x%02x bytes: %s" % (raw_off, len(code), code))
    pe.seek(raw_off)
    pe.write(code)

def main():

    args = parseCommandLine()

    print("=== Patching ForgedAlliance_base.exe to ForgedAlliance_ext.exe ===")

    shutil.copyfile('ForgedAlliance_base.exe', 'ForgedAlliance_ext.exe')
    
    print("Preparing dependencies...")

    # Run deps
    call(['python','emit_lua_hooks.py'])
    call(['python','emit_luaH_hooks.py'])
    call(['python','emit_msvc_import_table.py'])
    call(['python','emit_fixer_hooks.py'])
    call(['python','emit_duds.py'])
    
    call(['python','build_ext.py'])
    call(['python','emit_map_hooks.py'])
    
    # === Build the final PE ===
    print("Building final PE.")

    filename = 'ForgedAlliance_ext.exe'

    ext_section_offset = 0x00BDD000 # 3599

    pe = open(filename,'r+b')

    if not os.path.exists('build/'):
        os.makedirs('build/')

    hooks = [#'hook_odd_mem_alloc.s',
             ]#['hook_LoadSavedGame.s',
            #'hook_ArmyGetHandicap.s']#,
            #'hook_ValidateFocusArmyRequest.s']

    for _d in ['hooks','build/hooks']:
        for root, dirs, files in os.walk(_d):
            for f in files:
                fname = pjoin(root, f)
                if f.startswith('hook_') and f.endswith('.asm'):
                    hooks += [fname]
    
    for hook_i in range(len(hooks)):
        hook = hooks[hook_i]
        print('\r[%3d / %3d] Applying hook "%s"                     ' % (hook_i+1, len(hooks), hook), end='')
        apply_hook(pe, hook)

    print('\n')

    pe.seek(0x0)
    pe.write(open('FA_PE_start_handed.bin','rb').read())

    pe.seek(0xBDD000)
    pe.write(bytes([0]*0x100000))
    pe.seek(0xBDD000)
    with open('build/ext_sector_exe.bin','rb') as f:
        size = os.fstat(f.fileno()).st_size
        if size > 0x100000:
            raise RuntimeError('C compiled sector size too big.')
        else:
            pe.write( f.read() )
            
    pe.seek(0xCDD000)
    pe.write(bytes([0]*0x40000))
    pe.seek(0xCDD000)
    with open('build/ext_sector_data.bin','rb') as f:
        size = os.fstat(f.fileno()).st_size
        if size > 0x40000:
            raise RuntimeError('C compiled sector size too big.')
        else:
            pe.write( f.read() )
            
    # Patch-in the msvcr80 imports
    
    pe.seek(0xD1D000)
    pe.write(bytes([0]*0x1000))
    pe.seek(0xD1D000)
    pe.write(open('build/FA_msvc_imports.bin','rb').read())

    pe.seek(0xD1E000)
    pe.write(bytes([0]*0x10000))
    pe.seek(0xD1E000)
    pe.write(open('build/ext_sector_debug.bin','rb').read())
        

    pe.close()
    print("Done.")
    if args.output_file:
        shutil.move(filename, args.output_file)
        print("Moved",filename,"to",args.output_file)

if __name__ == "__main__":
    main()

    
