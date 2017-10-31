#-----------------------------------------------------------------------------
#
# Parsing help
#
#-----------------------------------------------------------------------------

# parse function to return the rest of the line
set scanrest[set_rest, parse]:{
    set_rest parse.0!;
    parse.0 = "";
    TRUE
}

set parse_nonblank_lines[
    _help="<fn> <text> - run ok = <fn> <line>! for non-blank lines in <text>",
    with_fn, text
]:{
    text != NULL {
        .ok = TRUE;
        forall (split "\n" text!) [line]:{
            line != "" {not (with_fn line!)}! {ok=FALSE}!
        }!;
        ok
    }!
}


