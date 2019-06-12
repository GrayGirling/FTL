#-----------------------------------------------------------------------------
# command line argument handling
#-----------------------------------------------------------------------------

set parse_codeitemstr[arg_wr,left]:{
    parse.scanwhite left!;
    if (parse.scancode arg_wr left!){TRUE}{
        parse.scanitemstr arg_wr left!
    }!
}
   
set split_args[line]:{
   .left = <line>;
   .args = <>;
   .arg = "";
   while {parse_codeitemstr @arg left!} {
       args.(len args!) = arg;
   }!;
   if (parse.scanempty left!) {args}{
       errorf "Unknown text on line - '%s'" <line>!;
       NULL
   }!
}

# parse <n> items then the rest of the line
set split_argrol[first_args, line]:{
   .left = <line>;
   .args = <>;
   .arg = "";
   .count = first_args;
   while {count _gt_ 0 {parse_codeitemstr @arg left!}!} {
       args.(len args!) = arg;
       count = count-1;
   }!;
   if count == 0 {
       parse.scanwhite left!;
       args.(len args!) = parse.scanned left!;
       args
   }{
       errorf "Too few arguments, syntax - <item>*%d <rest of line>"
           <first_args>!;
       NULL
   }!
}

set get_argv[cmin,cmax,help,line]:{
   .argv = split_args line!;
   .ln = len argv!;
   .ok = TRUE;
   ln lt cmin {
       ok = FALSE;
       errorf "Too few arguments (%d), syntax - %s" <ln,help>!;
   }!;
   ln gt cmax {
       ok = FALSE;
       errorf "Too many arguments (%d), syntax - %s" <ln,help>!;
   }!;
   # printf "get_argv %d..%d ok=%v %v\n"<cmin,cmax,ok, argv>!; 
   if ok {argv}{NULL}!
}


set checkstr[s]:{
   if (typeof s!) != type_str {
      errorf "Sorry, an argument is missing"<>!; FALSE
   }{ TRUE }!
}

set checkcode[s]:{
   .sty = typeof s!;
   .ok = (sty == type_code _or_ sty == type_closure);
   not ok {
       errorf "Sorry, an argument code block is missing (has %v type)"<sty>!
   }!;
   ok
}

set checkstrcode[s]:{
   .sty = typeof s!;
   .ok = not (sty != type_str {sty != type_code}!);
   sty == type_str {(len s!) == 0}! {ok = FALSE}!;
   not ok {errorf "Sorry, an argument item or code block is missing"<>!}!;
   ok
}

set strton[str]:{
   .p = parse.scan str!;
   .n = NULL;
   if (parse.scanintval @n p! { parse.scanempty p! }!) {n} {NULL}!
}

set cvtlog[cvtname,val]:{
    # printf "convert '%v' using %s\n"<val,cvtname>!; 
}

set convert []
set convert.code[s]:{cvtlog "code" s!;if (checkcode s!) {s}{NULL}!}
set convert.strcode[s]:{cvtlog "strcode" s!;if (checkstrcode s!) {s}{NULL}!}
set convert.str[s]:{cvtlog "str" s!;if (checkstr s!) {s}{NULL}!}
set convert.strne[s]:{
    cvtlog "strne" s!;
    if (checkstr s! {(len s!) _gt_ 0}!) {s}{NULL}!}
set convert.int[s]:{cvtlog "int" s!;if (checkstr s!) {strton s!}{NULL}!}
set convert.strint[s]:{
    cvtlog "strint" s!;
    if (checkstr s!) {
       .n = strton s!;
       if n == NULL { s } { n }!
    }{NULL}!
}

set syn[spec]:{
   .parts = select [s]:{ (len (split ">" s!)!) gt 1 } (split "<" spec!)!;
   .vec = <>;
   forall parts [s]:{ vec.(len vec!) = (split ">" s!).0; }!;
   convert.(vec)
}

set cmd_3item_rol[cvt, fn,help]:{
   cmd [line]:{
       .argv = split_argrol 2 line!;
       if (argv != NULL {(len argv!) _ge_ 3}!) {
           fn (cvt.0 argv.0!) (cvt.1 argv.1!) (cvt.2 argv.2!)!
       }{ errorf "Syntax %s"<help>! }!;
   } help!
}

set cmd_2item_rol[cvt, fn,help]:{
   cmd [line]:{
       .argv = split_argrol 1 line!;
       if (argv != NULL {(len argv!) _ge_ 1}!) {
           fn (cvt.0 argv.0!) (cvt.1 argv.1!)!
       }{ errorf "Syntax %s"<help>! }!;
   } help!
}

set cmd_1item[cvt, fn,help]:{
   cmd [line]:{
       .argv = get_argv 1 1 help line!;
       argv != NULL {fn (cvt.0 argv.0!)!}!;
   } help!
}

set cmd_2item[cvt, fn,help]:{
   cmd [line]:{
       .argv = get_argv 2 2 help line!;
       # printf "2item -> %v\n"<argv>!;
       if (argv != NULL {(len argv!) _ge_ 1}!) {
           # printf "cvt %v size %d\n"<domain cvt!, len cvt!>!;
           fn (cvt.0 argv.0!) (cvt.1 argv.1!)!
       }{ errorf "Syntax %s"<help>! }!;
   } help!
}

set cmd_3item[cvt, fn,help]:{
   cmd [line]:{
       .argv = get_argv 3 3 help line!;
       if (argv != NULL {(len argv!) _ge_ 2}!) {
           fn (cvt.0 argv.0!) (cvt.1 argv.1!) (cvt.2 argv.2!)!
       }{errorf "Syntax %s"<help>!}!;
   } help!
}


