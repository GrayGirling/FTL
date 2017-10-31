#-----------------------------------------------------------------------------
#
# whole file operations
#
#-----------------------------------------------------------------------------

set file_loaddata_max[openfn, maxsz, file]:{
    .data = NULL;
    .opened = openfn file "r"!;
    opened != NULL {
        data = io.read opened maxsz!;
        io.close opened!;
    }!;
    data
}


set file_savedata[openfn,file,data]:{
    .ok = FALSE;
    .opened = openfn file "w"!;
    opened != NULL {
        ok = io.write opened data!;
        io.close opened!;
    }!;
    ok
}

set loaddata file_loaddata_max io.binfile 10000000
set savedata file_savedata io.binfile

set loadtext file_loaddata_max io.file 10000000
set savetext file_savedata io.file


