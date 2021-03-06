set write[file, msg]:{
    .f = io.file file "w"!;
    f != NULL {io.write f ""+(msg)+("\n")!}!;
    io.close f!;
}
write "hello"  "echo hello world"
set include [name]:{
    .rdf = NULL;
    if (inenv sys.env "MYPATH"!) {
        rdf = io.pathfile sys.env.MYPATH name "r"!;
    }{  rdf = io.file name "r"!;
    }!;
    if (equal NULL rdf!) {
        echo "can't read file "+(str name!)!;
    } {
        .ret = parse.exec "" rdf!;
        io.close rdf!;
        ret
    }!
}
include "hello"
# hello world
set sys.env.MYPATH "."
include "hello"
# hello world
set sys.env.MYPATH ".."
include "hello"
# can't read file "hello" - assuming no file "hello" current