source
source ""
source "::://:::"

set write[file, msg]:{
    .f = io.file file "w"!;
    if f == NULL {
        echo "Failed to open file '$file' for writing"!;
    }{
        io.write f ""+(msg)+("\n")!
    }!;
    io.close f!;
}
write "ftltest.tmp"  "echo Running line from test\necho Finished in test"
source ftltest.tmp
source "ftltest.tmp"

set src[f]:{if TRUE {source f!}{}!;echo "Done now"!}
src "test"
