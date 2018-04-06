source
source ""
source "::://:::"

set write[file, msg]:{
    .f = io.file file "w"!;
    f != NULL {io.write f ""+(msg)+("\n")!}!;
    io.close f!;
}
write "test"  "echo Running line from test\necho Finished in test"
source test
source "test"

set src[f]:{if TRUE {source f!}{}!;echo "Done now"!}
src "test"
