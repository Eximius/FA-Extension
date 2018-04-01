/* Entry point; at the very start of the binary */
/*ENTRY(_ext_lua_LoadSavedGameHook)*/

align_size = 0x1000; /* squeeeeeze */

MEMORY
{
	dummy (RWX) : ORIGIN = 0x500000, LENGTH = 0x5000
    ext (RX) : ORIGIN = 0x128B000, LENGTH = 0x100000
    ext_data (RW) : ORIGIN = 0x138B000, LENGTH = 0x40000
    ext_idata (R) : ORIGIN = 0x138C000, LENGTH = 0x5000
}
/* Binary sections */
SECTIONS
{   
	. = SIZEOF_HEADERS;
	. = ALIGN(align_size);

	. = 0x500000;

	.dummy_0 : {
		*(.dummy_0)
	}
	.dummy_1 : {
		*(.dummy_1)
	}
	.dummy_2 : {
		*(.dummy_2)
	}
	.dummy_3 : {
		*(.dummy_3)
	}
	.dummy_4 : {
		*(.dummy_4)
	}
	.dummy_5 : {
		*(.dummy_5)
	}

	/* Code */
	.ext : 
	{
		*(.ext)
		EXCLUDE_FILE (lj_*) *(.text*)

		. = ALIGN(0x10000);
		lj_*(.text*)
	} >ext
	
	/* Read-only data. */
	.ext_data :
	{
        *(.idata*)
		*(.rdata*)
		*(.data)
		*(.bss)
		
		*(COMMON)
	} >ext_data
	
	/*.= 0x138C000;
	
	.ext_idata :
	{
	} */
	
	.debug ALIGN(align_size) : {
		*(.debug*)
	}
	
	/* Discard the taint */
	/*/DISCARD/ :
	{
        *(.debug*)
	}*/
	
	/* Read-write data (initialized) */
	/*.data BLOCK(align_size) :
	{`
		*(.data)
	} Haven't made sector for this, so no globals, for now*/

	/* Read-write data (uninitialized) and stack */
	/*.bss BLOCK(align_size) :
	{
		*(COMMON) 
		*(.bss)
	} nor this */
}
