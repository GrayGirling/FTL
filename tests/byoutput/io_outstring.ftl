set s io.outstring [out]:{ forall <..5> {io.write out "line\n"!}! }!
echo $s
#line
#line
#line
#line
#line
set mystrf[fmt,args]:{io.outstring [out]:{io.fprintf out fmt args!}!}
mystrf "hi - %s" <"you">
# "hi - you"
