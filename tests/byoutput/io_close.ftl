io close (io.file "test" "w"!)
set log[msg]:{
    .logstr = io.file "log" "w"!;
    io.write logstr msg!;
    io.write logstr "\n"!;
    io.close logstr!;
}
set cat[file]:{
    .logstr = io.file file "r"!;
    .txt = NULL;
    logstr != NULL {
        txt = io.read logstr 1000000!;
        io.close logstr;
    }!;
    txt
}
log "this"
cat "log"
