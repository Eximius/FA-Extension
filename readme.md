Forged Alliance Extension
===========

Hello! And welcome to the wonderful extravagant project of extending the functionality of a piece of software through an endless series of horrenduously horrible hacks! Wherein, we find ourselves, essentially, recompiling, reorganizing the old dilapidated and not in any way creating something innovatively exciting! Many scholars would even call this the point at which creativity and intellect go to die. Surely babbling on about all this, which amounts mostly to trivial existential dread in a jaded an sarcastic introduction to a small project is palpably silly. Without further ado...

This tiny project, the Forged Alliance Extension is a large series of hacks to extend Forged Alliance's (patch 3599) functionality.

Requirements
	
	nasm - http://www.nasm.us/
	gcc - i686 target http://mingw-w64.sourceforge.net/download.php#win-builds
	[patched version of luajit](https://github.com/Eximius/gpg-luajit/tree/odd-develop)


The folder scheme is as follows:
```
├── <files> scripts and other core files.
├── build
      Build files are stored here.
├── hooks
│   └── _map_hooks.lst - A file listing trivial hooks to be linked to functions in 
│        the ext_sector module (built from src/) by name.
│       Contains hand-coded hook files, automatically globbed and hooked in.
├── notes
│       Should hopefully contain useful notes that relate to larger problems in decompiling.
├── patch.py - main patch script
├── src
        Source for the ext_sector.dll module which gets plopped into the resulting executable to provide a solid base for new code.
```
The creation of the patched executable occurs in stages.

The useful script do_the_thing.sh:
```
# Does the thing

test_bin_dir='/mnt/D/SupCom FA Lite/bin/'

python emit_msvc_import_table.py

python patch.py -c \
        && cp ForgedAlliance_ext.exe "$test_bin_dir" \
        && i686-w64-mingw32-nm -g ForgedAlliance_ext.exe \
        && i686-w64-mingw32-objcopy --only-keep-debug build/ext_sector.exe build/ext_sector.dbg \
        && cp build/ext_sector.dbg "$test_bin_dir"

        # && objdump -h build/ext_sector.exe \
        # && ls --color -lah build \
```

First an additional import table that is to be inserted into the new exe is created containing new required symbols from msvc80.

The patch.py is then called, which creates the resulting exe. 

do_the_thing.sh facilitates easy debugging by quickly doing everything needed (i.e. copy, check, make debug symbols file)


In patch.py:
```
    # Run deps
    call(['python','emit_lua_hooks.py'])
    call(['python','emit_luaH_hooks.py'])
    call(['python','emit_msvc_import_table.py'])
    call(['python','emit_fixer_hooks.py'])
    call(['python','emit_duds.py'])
    
    call(['python','build_ext.py'])
    call(['python','emit_map_hooks.py'])
```

These dependencies are called in order and setup the appropriately named things. emit_**.py means it emits hook.asm files that will be hooked in by patch.py.

emit_map_hooks.py checks hooks/_map_hook.lst and maps them to functions in ext_sector.dll (built by build_ext.py) then emits the hook files.

How do I dev
=========

The usual process of development is to pose the question, "what should work?" and then starting up FA.exe under ida and finding that it doesn't work. Breaking and debugging, which with enough willpower should give you a good idea of "what to do next".

As of writing this, the main problem was that when booting up a map/game, the engine would try to register RTypes (their own reflection types) and would happen to fail due to luajit not being fully immersed in all the code. Essentially, GPG used LuaPlus, which can have its inline member functions inlined everywhere. However, happily, they seem to have built their executable with an incredible lack of optimisations. Thus, just mostly the not having found all used functions of LuaPlus causes problems.

There are tiny differences from LuaPlus (lua 5.0) to luajit (lua 5.1). So I use a [patched version of luajit](https://github.com/Eximius/gpg-luajit/tree/odd-develop)

You can always study Mohoengine.dll gpgcore.dll gpggal.dll SupremeCommander.exe (from the first game) to see how their code looks (thos dlls have a lot of symbols), as in ForgedAlliance.exe there are no symbol names (no imports/exports).

Ping me on somewhere if you need the database. 

P.S.
===============

When reverse-engineering moho/FA, do check src/moho.h for how the structure look (you can load them in ida if they're fully defined). Also, notes/type.txt has the real, unambiguous constants.
