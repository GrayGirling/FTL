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

set opt [
    op1 = [num]:{ printf "num is %v\n" <num>!; },
    op2 = [str, str2]:{ printf "str is %v, str2 is %v\n" <str, str2>!; }
]

