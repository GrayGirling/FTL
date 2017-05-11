# Make sure the bool values exist
eval TRUE
eval FALSE

TRUE {echo "prints"!}           # returns FALSE (NULL returned by body)
FALSE {echo "doesn't print"!}   # returns FALSE

TRUE {echo "prints"!;TRUE}! {echo "prints again"!}       # returns TRUE
TRUE {echo "prints"!;FALSE}! {echo "doesn't print"!}     # returns FALSE
FALSE {echo "doesn't print"!; TRUE}! {echo "doesn't print"!}   # returns FALSE
FALSE {echo "doesn't print"!; FALSE}! {echo "doesn't print"!}  # returns FALSE

invert TRUE
invert FALSE

equal TRUE TRUE
equal TRUE FALSE
equal FALSE TRUE
equal FALSE FALSE

# same thing using pre-defined operator
eval TRUE == TRUE
eval TRUE == FALSE
eval FALSE == TRUE
eval FALSE == FALSE

set ifeval[bool]:{ if bool {"if-TRUE"} {"if-FALSE"}! }
ifeval TRUE
ifeval FALSE

