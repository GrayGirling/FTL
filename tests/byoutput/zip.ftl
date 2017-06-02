zip <0..6> <"even", "odd">
#<"even", "odd", "even", "odd", "even", "odd", "even">
zip <3..0> <"abc", "def", "ghi", "jk">
#<"jk", "ghi", "def", "abc">
zip <0..9> 0
#<0, 0, 0, 0, 0, 0, 0, 0, 0, 0>
zip <"c","c#","d","d#","e","f","f#","g","g#","a","a#","b"> <0..11>
# [c=0, "c#"=1, d=2, "d#"=3, e=4, f=5, "f#"=6, g=7, "g#"=8, a=9, "a#"=10, b=11]
set invert[x]:{zip (range x!) (domain x!)!}
invert [c=0, "c#"=1, d=2, "d#"=3, e=4, f=5, "f#"=6, g=7, "g#"=8, a=9, "a#"=10, b=11]
#<"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b">
invert [a=4,b=3,c=9,d=2]
#<2="d", "b", "a", 9="c">
