set write[file, msg]:{
    .f = io.file file "w"!;
    f != NULL {io.write f ""+(msg)+("\n")!}!;
    io.close f!;
}
write "log"  "=== a line of logging\n=== followed by another"
set f io.file "log" "r"!
io read f 25
