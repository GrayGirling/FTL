set vec <2,6,9>
set ids [a=6,b=2,h=9]
eval vec.0
#2
eval ids.h
#9
eval ids."h"
eval vec.(ids.b)
eval vec.<2,1,2>
#<9, 6, 9>
eval vec.[f=2,g=1,h=2]
#[f=9, g=6, h=9]
eval ids.<"b","a">
#<2, 6>
eval ids.[q="h", l="a"]
#[q=9, l=6]
eval vec.<4>
#<>
eval vec.<4,0,1>
#<1=2, 6>
eval vec.(ids)
#["a"=, "b"=9, "h"=]
eval ids.[x="i", y="b"]
#["x"=, "y"=2]
inenv ids.[x="i", y="b"] "x"
#FALSE
inenv ids.[x="i", y="b"] "y"
#TRUE
