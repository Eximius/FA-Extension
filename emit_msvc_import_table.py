import io
import struct
import datetime

# Generate msvcrt.dll imports

import_sector_base = 0x00FDB000

#
#   This script puts the import_descriptor list at 0x300
#

dll_name = 'msvcrt.dll'

import_list = [
'_errno',
'_iob',
'abort',
'cosh',
'exit',
'fclose',
'feof',
'ferror',
'fflush',
'fopen',
'fputc',
'fputs',
'fread',
'ldexp',
'memcmp',
'memmove',
'putchar',
'sinh',
'sprintf',
'strchr',
'strcmp',
'strcpy',
'strerror',
'strlen',
'strncmp',
'strncpy',
'strstr',
'strtoul',
'tanh',
'vfprintf',
'fwrite',
'printf',
'memcpy',
'malloc',
'realloc',
'free',
'memchr',
'strpbrk',
'fgets',
'_pclose',
'fscanf',
'getc',
'ungetc',
'clearerr',
'fseeko64',
'ftello64',
'setvbuf',
'_popen',
'tmpfile',

'_onexit'
]

import_sector = io.BytesIO(bytes([0]*0x1000))

import_sector.seek(0x0)

dll_addr = 0x0
import_sector.write(dll_name.encode() + bytes([0x0]))

import_by_name_addr = []
def emit_import_by_name():
    for imp in import_list:
        import_by_name_addr.append(import_sector.tell())
        import_sector.write(bytes([0, 0]) + imp.encode() + bytes([0]))

def emit_thunk_list():
    assert import_sector.tell() < 0x1f0
    import_sector.seek(0x200)
    for addr in import_by_name_addr:
        import_sector.write(struct.pack("<L", import_sector_base+addr))

def emit_import_descriptor():
    assert import_sector.tell() < 0x2f0
    import_sector.seek(0x300)

    old_iat = \
'''
60 8F B2 00 00 00 00 00 00 00 00 00 E6 93 B2 00
E8 E5 80 00 CC 89 B2 00 00 00 00 00 00 00 00 00
A0 94 B2 00 54 E0 80 00 B0 8B B2 00 00 00 00 00
00 00 00 00 68 9F B2 00 38 E2 80 00 68 8F B2 00
00 00 00 00 00 00 00 00 2E AB B2 00 F0 E5 80 00
3C 8A B2 00 00 00 00 00 00 00 00 00 2C B1 B2 00
C4 E0 80 00 E8 92 B2 00 00 00 00 00 00 00 00 00
A0 B1 B2 00 70 E9 80 00 78 89 B2 00 00 00 00 00
00 00 00 00 18 B3 B2 00 00 E0 80 00 24 8F B2 00
00 00 00 00 00 00 00 00 26 B4 B2 00 AC E5 80 00
A4 93 B2 00 00 00 00 00 00 00 00 00 02 B5 B2 00
2C EA 80 00 3C 92 B2 00 00 00 00 00 00 00 00 00
A6 B5 B2 00 C4 E8 80 00 74 93 B2 00 00 00 00 00
00 00 00 00 5C B6 B2 00 FC E9 80 00 9C 93 B2 00
00 00 00 00 00 00 00 00 76 B6 B2 00 24 EA 80 00
DC 92 B2 00 00 00 00 00 00 00 00 00 B6 B6 B2 00
64 E9 80 00 E0 89 B2 00 00 00 00 00 00 00 00 00
52 B8 B2 00 68 E0 80 00 0C 93 B2 00 00 00 00 00
00 00 00 00 04 BB B2 00 94 E9 80 00 34 8A B2 00
00 00 00 00 00 00 00 00 12 BB B2 00 BC E0 80 00
04 93 B2 00 00 00 00 00 00 00 00 00 30 BB B2 00
8C E9 80 00 28 92 B2 00 00 00 00 00 00 00 00 00
7C BB B2 00 B0 E8 80 00
'''
    for line in old_iat.splitlines():
        import_sector.write(bytes.fromhex(line))

    # Add our own imports to the list
    import_sector.write(struct.pack('<LLLLL', 
         import_sector_base+0x200, 0, 0, 
          import_sector_base, import_sector_base+0x200))

def emit_asm_link():
    with open("build/msvc_imports.asm", 'w') as f:
        f.write(
'''; Generated table on %s
;.intel_syntax noprefix
;.arch i686
;.text

bits 32

; Symbols required from kernel32.dll and msvcr80.dll

section .text
align 0x4

global __imp__GetLastError@0
__imp__GetLastError@0 equ 0xc0e53c


global __imp__SetLastError@4
__imp__SetLastError@4 equ 0xC0E43C


global __imp__VirtualAlloc@16
__imp__VirtualAlloc@16 equ 0xC0E458


global __imp__VirtualFree@12
__imp__VirtualFree@12 equ 0xC0E2AC

extern eat_shit_and_die
global __imp__VirtualProtect@16
__imp__VirtualProtect@16:
    call eat_shit_and_die
    ret

global __imp__VirtualQuery@12
__imp__VirtualQuery@12 equ 0xC0E44C

align 0x4

; MSVC import jumptable

section .idata

msvc_imports equ 0x13DB200

''' % datetime.datetime.now())

        for i in range(len(import_list)):
            imp = '__imp__'+import_list[i]
            if import_list[i][0] == '_':
                # Variable
                f.write(
'''
global %s
%s equ 0x%x
''' % (imp, imp, 0x400000 + import_sector_base + 0x200 + i*4))
            else:
                f.write(
'''
global %s
%s:
    jmp [msvc_imports+0x%x]
''' % (imp, imp, i*4))

emit_import_by_name()
emit_thunk_list()
emit_import_descriptor()

emit_asm_link()

with open("build/FA_msvc_imports.bin", 'wb') as f:
    f.write(import_sector.getvalue())
