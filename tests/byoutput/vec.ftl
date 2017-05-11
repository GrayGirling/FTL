eval <>
eval <100>
eval <1000,2000>
eval <"a">

eval <1,2,7=9>
eval <5=9,10, 11>
eval <3=3,7=7>

eval <"a"=4> # error

eval <10..13>
intseq 10 NULL 13
eval <13..10>
intseq 13 NULL 10
eval <4, 7 .. 23>
intseq 4 7 23
eval <1..>
intseq 1 NULL NULL
eval <..>
intseq NULL NULL NULL
eval <1, 0 ..>
intseq 1 0 NULL     # bug?
eval <..8>
intseq NULL NULL 8

# impossible sequences just return nothing
eval <4, 2 .. 12>
intseq 4 2 12
eval <4, 8 .. -12>
intseq 4 8 (-12)

equal NULL (typeof <4, 2 .. 12>!) # TRUE
equal (type "dir"!) (typeof <>!)   # TRUE
len <4, 8 .. -12> # error

len <>
len <1,2,7=9>
len <5=9,10, 11>
len <3=3,7=7>
len <10..13>
len <13..10>
len <4, 7 .. 23>
# len <1..> # - bad idea .. takes a long time

set v <..10>
set v.(len v!) 11
eval v

eval <>.0    # undefined
eval <>.(-1) # undefined
eval <>.1    # undefined

eval <42>.0   
eval <42>.(-1) # undefined
eval <42>.1    # undefined

eval <3=33,7=77>.2 # undefined
eval <3=33,7=77>.3
eval <3=33,7=77>.4 # undefined
eval <3=33,7=77>.7
eval <3=33,7=77>.8 # undefined

eval <4, 8 .. 23>.0
eval <4, 8 .. 23>.1
eval <4, 8 .. 23>.2
eval <4, 8 .. 23>.3
eval <4, 8 .. 23>.4
eval <4, 8 .. 23>.5 # undefined
eval <4, 8 .. 24>.5 
