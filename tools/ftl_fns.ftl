# Prolog for FTL tool
#
# Assuming this file is in ~/tools/ftl_fns.ftl, This file can be copied into a
# long string suitable for incorporating into penv.c using the command:
#
#        ./ftl -q -- io filetostring tools/ftl_fns.ftl > tools/ftl_fns.str
#
# To test a new version use
#
#        FTL_PROLOG=tools/ftl_fns.ftl ./ftl
#

set printf io.fprintf io.out
set errorf io.fprintf io.err

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

