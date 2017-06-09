set done FALSE
set once[fn]: { if (done) { done=TRUE; fn! } {}! } 
#{   rc = fn!;
#    if (rc.error_occured) {echo "ERROR"!; FALSE} {TRUE}!
#}
eval FALSE {echo "never run"!}!
# FALSE
equal TRUE FALSE
equal FALSE FALSE
notequal TRUE FALSE
notequal FALSE FALSE
eval not FALSE
eval invert FALSE!
logand FALSE FALSE
logand FALSE TRUE
logor FALSE FALSE
logor FALSE TRUE

typeof FALSE
