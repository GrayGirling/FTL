set inrange[x,low,high]: { 1 and (moreeq x low!) and (lesseq x high!) }
inrange 0 3 6
inrange 3 3 6
inrange 4 3 6
inrange 6 3 6
inrange 7 3 6
inrange "ab" "a" "b"
inrange basetype.dir basetype.nul basetype.string
inrange basetype.string basetype.nul basetype.dir
