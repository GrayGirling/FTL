set sin io.instring "In" "r"!
io getc sin
#"I"
io getc sin
#"n"
io getc sin
#
set rev[st]:{
    .out="";
    .ch=NULL;
    while {NULL != (ch=io.getc st!)} {
        out=""+(ch)+(out);
    }!;
    out
}
set sin io.instring "able was I ere" "r"!
rev sin
# "ere I saw elba"
