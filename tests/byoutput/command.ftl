set doit "echo"
command $doit some arguments for $doit
# some arguments for echo
command return 2+2
#4

set PROFORMA[cond,then]:{cond {command then!}!}
set WHEN[cond,then]:{cond {sourcetext then!}!}
WHEN TRUE {
    set scope 2
    echo scope is $scope
}
# scope is 2
set scope 1
PROFORMA TRUE {
    set scope 2
    echo scope is $scope
}
# scope is 1
