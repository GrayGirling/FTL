cmp 9 1
#1
cmp "Astring" "Bstring"
#-1
cmp "string\0_B" "string\0_A"
#1
cmp <1> <1>
#-1
set dir <1>
cmp dir dir
#0
eval cmp NULL ({}!)!
cmp (typeof <>!) (typeof []!)

