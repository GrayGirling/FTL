set onetwo_fn[s]:{ (split " " s!).<0,1> }
set onetwo cmd onetwo_fn "<first> <second> .. return 1st and 2nd items"!
help onetwo
#onetwo <first> <second> .. return 1st and 2nd items
onetwo beaky mick titch
# <"beaky", "mick">
eval onetwo "-o file -- opts"!
# <"-o", "file">
