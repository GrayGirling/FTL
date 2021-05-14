sourcetext "echo executing..."
# executing...
sourcetext {
    echo executing…
}
# executing…

set state "executing"
sourcetext " enter [state=\"running\"]; echo ${state}...; "
# executing...
sourcetext { enter [state="running"]; echo ${state}...; }
# running...
sourcetext " enter [state=\"running\"]; echo \${state}...; "
# running…

set test[]:{ sourcetext "echo executing..."!; echo "Started"! }
test
# executing...
# Started

set IF[cond,then,else]:{ 
    if cond {sourcetext then!}{sourcetext else!}! 
}
IF TRUE { 
     echo true
     return 1 
}{ 
     echo false
     return 0 
}
# true
# 1
