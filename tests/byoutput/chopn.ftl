set first[n,str]:{ (chopn 1 n str!).0 }
set last[n,str]:{ (chopn 1 (-n) str!).0  }
set allbutfirst[n,str]:{ (chopn 1 n str!).1 }
set allbutlast[n,str]:{ (chopn 1 (-n) str!).1 }
first 3 "abcdefghij"
# "abc"
last 3 "abcdefghij"
# "hij"
allbutfirst 3 "abcdefghij"
# "defghij"
allbutlast 3 "abcdefghij"
# "abcdefg"
chopn 1 [op=1,header=7] "3headerhpacketcontent"
# <[op="3", header="headerh"], "packetcontent">
