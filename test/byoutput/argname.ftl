argname [b=1,a]::{a+b}
argname set

set needs_argument[fn]:{NULL != (argname fn!)}
needs_argument needs_argument
needs_argument []:{echo "no"!}
