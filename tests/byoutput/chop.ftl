chop 3 "abcdefghijk"
chop -3 "abcdefghijk"
set rev[x]:{x.<(len x!)-1..0>}
join "," (rev (chop (-3) (strf "%d" <10000000>!)!)!)
