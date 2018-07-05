# example
set line2[]:{echo "on line 2"!}
set line3[]:{echo "on line 3"!}
set line4[]:{echo "on line 4"!}
set line5[]:{
    echo "on line 4"!
}

set line9[]:{}


set f1[]:{
    echo "f1 start"!;
    f2 TRUE!;
    echo "f1 stop"!;
}


set f2[bool]:{
    echo "f2 start"!;
    if bool {
        for <1..4>[i]:{ # first line
            # line two
            f3 i!
        }!
    }{
        f4 TRUE!
    }!;
    echo "f2 stop"!;
}



f1

echo complete

