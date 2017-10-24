#-----------------------------------------------------------------------------
#
# string functions
#
#-----------------------------------------------------------------------------

set substr[_help="<len> <pos> <str> - return substring of string",
           len, pos, str]:{
    if len _le_ 0 {""}{
        join NULL (split NULL str!).<pos .. pos+len-1>!
    }!
}

set _ch_decimal <'0','1','2','3','4','5','6','7','8','9'>
set _decimal zip _ch_decimal <0..9>!
set isdigit inenv _decimal

# delete all occurrences of txt from substr
set strdrop[substr, txt]:{join NULL (split substr txt!)!}


set strtoi[txt]:{
    .o = parse.scan txt!;
    .i = NULL;
    if (parse.scanintval @i o! {parse.scanempty o!}!) {i} {NULL}!
}

set strtobool[txt]:{
    if txt == "TRUE" {TRUE}{
        if txt == "true" {TRUE}{
            if txt == "FALSE" {FALSE}{
                if txt == "false" {FALSE}{
                    .n = strtoi txt!;
                    if n != NULL {n != 0} {FALSE}!
                }!
            }!
        }!
    }!
}



