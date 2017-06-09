set done FALSE
set once[fn]: { if (done) { done=TRUE; fn! } {}! } 
#{   rc = fn!;
#    if (rc.error_occured) {echo "ERROR"!; FALSE} {TRUE}!
#}
eval TRUE {echo "always run"!}!
# always run
# FALSE
equal FALSE TRUE 
equal TRUE TRUE
notequal TRUE FALSE
notequal TRUE TRUE
eval not FALSE
eval invert TRUE!
logand TRUE TRUE
logand FALSE TRUE
logor FALSE TRUE
logor TRUE TRUE

typeof TRUE

eval TRUE {0}!
eval TRUE {TRUE}!
eval TRUE {FALSE}!
eval TRUE {NULL}!
eval TRUE {;}!
eval TRUE {}!
