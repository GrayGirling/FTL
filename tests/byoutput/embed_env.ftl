# embedded closures

set oobj[_help="- open obj",a]:{
   [this=[_help="- say this",a]:{echo "this"!;io.fprintf io.out "a=%v\n"<a>!;},
    h=_help,
   ]
}

set o oobj "ping"!
echo oobj help:
o help
o this "trying"

# OK
set bobj[_help="- open obj",a]:{
   [this=[_help="- say this",a]]
}

# bad
set cobj[_help="- open obj",a]:{
   [this=[_help="- say this",a]:{}]
}

# OK
set dobj[_help="- open obj",a]:{
   [this=[_help="- say this",a]::{}]
}

# bad
set eobj[_help="- open obj"]:{
   [this=[_help="- say this"]:{}]
}

# bad
set fobj[var="bad"]:{
   [this=[var="good"]:{}]
}

# bad
set gobj[var="bad"]:{([var="good"]:{}).var}



#------------------------------------------

set nobj [var="bad"]:{}

eval nobj  # doesn't show 'var' but nobj.var works