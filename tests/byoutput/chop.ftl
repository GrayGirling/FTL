chop 3 "abcdefghijk"
# <"abc", "def", "ghi", "jk">
chop -3 "abcdefghijk"
# <"ijk", "fgh", "cde", "ab">
set rev[x]:{x.<(len x!)-1..0>}
join "," (rev (chop -3 (strf "%d" <10000000>!)!)!)
# "10,000,000"
chop [word=-4] "thisand that"
# <[word="that"], [word="and "], [word="this"]>
chop [first=<1,1>,body=6] "* thing1**thing2+*thing3"
# <[first=<"*", " ">, body="thing1"], [first=<"*", "*">, body="thing2"], [first=<"+", "*">, body="thing3"]>
join NULL (chop <-5,-4,-2,-4,-2,-4,-5> "able was I ere I saw elba "!)
# "elba saw I ere I was able "
