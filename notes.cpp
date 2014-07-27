// Python Function I made to try to define unreferenced code sections as functions
// (To make all the IDA disassembly blue)
def define_unreferenced_funcs():
    MINUS_ONE = 0xffffffffffffffff
    pos = ScreenEA()
    end = GetSegmentAttr(pos, SEGATTR_END)

    while pos <= end:
        #print("At 0x%08x" % pos)
        if GetFunctionAttr( pos, FUNCATTR_START)  == MINUS_ONE:
            #print("Found undefined function at 0x%08x" % pos )
            fend = FindFuncEnd(pos)
            if fend == MINUS_ONE:
                print("Failed to find function end.")
                Jump(pos)
                return
            MakeFunction(pos, fend)
            pos = fend
        else:
            pos = FindFuncEnd(pos)
        
        #print("FuncEnd at 0x%08x" % pos)
        if not isCode(idaapi.getFlags(pos)):
            pos = FindCode(pos, SEARCH_DOWN)

// Command I use to test FA
ForgedAlliance_ext.exe /map "Cobalt Valley v1" /windowed 1024 768 /EnableDiskWatch /showlog

// Notes on disassembly work:

unknown_libname_187 overwrites UserArmy validCommandSource list