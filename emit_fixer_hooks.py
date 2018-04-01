import io
import os
import struct
import datetime

from common import *

lua_env_push_addr = [
    0x4d2f8c,
    #0x90a16f,
    #0x90d67b,
    #0x90d6b2,
    #0x90e002,
    #0x90e49b,
    #0x90e988,
    #0x90eb6c,
    #0x90ebdc,
    #0x90ef3c,
    #0x90f078,
    #0x90f1a7,
    #0x90f1d5,
    #0x90f208,
    #0x90f53d,
    #0x90f625,
    #0x911d51,
    #0x9199b8,
    #0x919d1d,
    #0x926762

    # 0x4d32ac,
    # 0x90a90f,
    # 0x90de1b,
    # 0x90de52,
    # 0x90e7a2,
    # 0x90ec3b,
    # 0x90f128,
    # 0x90f30c,
    # 0x90f37c,
    # 0x90f6dc,
    # 0x90f818,
    # 0x90f947,
    # 0x90f975,
    # 0x90fcdd,
    # 0x90fdc5,
    # 0x9124f1,
    # 0x91a168,
    # 0x91a4cd,
    # 0x926f12
]   

for root, dirs, files in os.walk("build/hooks/", topdown=False):
    for name in files:
        if name.startswith('hook_fix_lua_env_'):
            os.remove(os.path.join(root, name))

for i in range(len(lua_env_push_addr)):
    addr = lua_env_push_addr[i]
    write_hook("hook_fix_lua_env_usage_%d.asm" % i,
'''; HOOK fix_lua_init     ROffset = 0x%x
bits 32
org 0x%x

push 0xFFFFD8EE
''' % (addr - 0x400000, addr))
