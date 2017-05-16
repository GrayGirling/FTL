domain [a=3, b="asdf", "c"=[], d]
# <"a", "b", "c">
domain [a=3, b="asdf"]::["c"=[], d]
join ", " (domain [a=3, b="asdf", "c"=[], d]!)
# "a, b, c"
set rndelement[env]: { env.((domain env!).(rnd (len env!)!)) }
#rndelement [a=3, b="asdf", "c"=[], d]
#"asdf"
#rndelement [a=3, b="asdf", "c"=[], d]
#3
#rndelement [a=3, b="asdf", "c"=[], d]
#[]

domain <1, 2, 4=4, 5>
domain <10..14>
# domain <10..14>::<1, 2, 4=4, 5>
domain [a=3, b="asdf"]::["c"=[], d]::<1, 2, 4=4, 5>
