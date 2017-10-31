#-----------------------------------------------------------------------------
#
# Errors
#
#-----------------------------------------------------------------------------

set errstr[e]:{if (e _lt_ 256 _and_ e _ge_ 0) {rl.err.(e)} {"<invalid error>"}!}

set errset[msg]:{
   rl.lasterrstr = msg;
   rl.lasterr = -1;
}

set why[]:{printf "Last error (%v) - %s\n"<rl.lasterr, if NULL == rl.lasterrstr {"OK"} {rl.lasterrstr}!>!;}


