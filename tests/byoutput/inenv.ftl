inenv [a=3,b=8,c=9] "d"
inenv [a=3,b=8,c=9] "c"
inenv [a=3]::[b=8,c=9] "a"
inenv [a=3]::[b=8,c=9] "c"

inenv []:[] "inenv"

inenv <1,2,3> 4
inenv <1,2,3> 2
inenv <1,2,3>::<7,8> 2
inenv <1,2,3, 6=7,8> 1
inenv <1,2,3, 6=7,8> 4
inenv <1,2,3, 6=7,8> 7
#inenv <1,2,3>::<7,8> 4
#inenv <1,2,3>::<7,8> 8

inenv <1..4> 0
inenv <1..4> 4
#inenv <1..4>::<9> 4
#inenv <1..4>::<9..12> 5
#inenv <1..4>::<9..12> 9

