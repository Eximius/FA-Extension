#!/bin/bash

set -e

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