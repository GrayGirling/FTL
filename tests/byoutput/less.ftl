set min[a,b]:{ if (less a b!){a}{b}! }
min 0 0
min 5 6
min "aword" "bword"
min "a" "ab"
min [a=5] [b=33]
min <1> <2>
min NULL 8
min 8 NULL
min basetype.int basetype.nul
