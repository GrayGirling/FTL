#-----------------------------------------------------------------------------
#
# events
#
#-----------------------------------------------------------------------------

# use as 'set myevent event!'
set event []:{
   .list = [];
   [  cause  = [_help="- cause this event"]:{
                   forall list [f,n]:{f != NULL {f!}!;}!
               },
      causes = [_help="<name> <fn> - register a <fn> this event causes",
                 name,fn]:{list.(name) = fn;},
      forget = [_help="<name> - forget the named function",
                 name]:{list.(name) = NULL;},
      clear  = [_help="- forget the functions this event causes"]:{list = [];},
      forall = forall list,
      active = []:{.a=<>; forall list [f,n]:{f != NULL {a.(len a!)=n}!}!; a}
   ]
}

# make an event to represent that we need to re-evaluate any constants
set rescan event!


