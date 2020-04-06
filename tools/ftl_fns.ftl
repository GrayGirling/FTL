# Prolog for FTL tool
#
# Assuming this file is in ~/tools/ftl_fns.ftl, This file can be copied into a
# long string suitable for incorporating into penv.c using the command:
#
#        ./ftl -np -q -- io filetostring tools/ftl_fns.ftl > tools/ftl_fns.str
#
# To test a new version use
#
#        FTL_PROLOG=tools/ftl_fns.ftl ./ftl
#

set codeid "ftl"

set printf io.fprintf io.out
set errorf io.fprintf io.err
set printf_hb[hbf,fmt,vals]:{.hb=int_fmt_hexbits hbf!;printf fmt vals!;int_fmt_hexbits hb!;}
set printfd printf_hb (-1)
set printfx printf_hb (0)
set printx[val]:{printfx "%v\n"<val>!;}
set print[val]:{printfd "%v\n"<val>!;}

#-----------------------------------------------------------------------------
# script paths
#-----------------------------------------------------------------------------

set basename[path]:{
   .pel = split sys.fs.sep path!;
   .elcount = len pel!;
   if (elcount gt 0) { pel.(elcount-1) } { "" }!
}

set dirname[path]:{
   .pel = split sys.fs.sep path!;
   .elcount = len pel!;
   if (elcount gt 1) {
      pel.(elcount-1)=NULL; join sys.fs.sep pel!
   } {
      sys.fs.thisdir
   }!
}

set path_startwith[path,dir]:{
   .plen = len path!;
   if plen == 0 { dir } { join sys.shell.pathsep <dir, path>! }!
}

set path_addstart[pathenv,dir]:{
   .path = if (inenv sys.env pathenv!) {sys.env.(pathenv)}{""}!;
   sys.env.(pathenv) = path_startwith path dir!;
   # printf "env %s now '%s'\n"<pathenv,sys.env.(pathenv)>!;
}

if (sys.fs.home != NULL) {
   path_addstart "FTL_PATH" (join sys.fs.sep <sys.fs.home, "."+(codeid)>!)!;
}{}

######################

set op_errors ""

set operror[msg]:{ op_errors = ""+(op_errors)+(msg)+"\n"; echo msg!; }

set scanonly[errfn,scanfn,str]:{
    .par=parse.scan str!;
    if ((scanfn par!) {parse.scanempty par!}!) {TRUE} {errfn!;FALSE}!
}

set withint[key,outn,str]:{
    scanonly []:{
        operror (strf "option %s (%v) must be a decimal number"<key, str> !)!} \
            (parse.scanint outn) str!
}

set withnum[key,outn,str]:{
    scanonly []:{operror (strf "option %s (%v) must be a number"<key, str>!)!}\
        (parse.scanintval outn) str!
}

set option_n 0
set option_i 0

set opt [
    op1 = [num]:{ printf "num is %v\n" <num>!; },
    op2 = [str, str2]:{ printf "str is %v, str2 is %v\n" <str, str2>!; },
    n = withint "-n" @option_n,
    i = withnum "-i" @option_i,
]

