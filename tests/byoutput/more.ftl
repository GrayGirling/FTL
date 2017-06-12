set max[a,b]:{ if (more a b!){a}{b}! }
max 0 0
max 5 6
max "aword" "bword"
max "a" "ab"
max [a=5] [b=33]
max <1> <2>
max NULL 8
max 8 NULL
max basetype.int basetype.nul
