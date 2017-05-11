set data [ads=3,b=9]

set dgetall []:{echo "(getall)"!;data}
set dget   [n]:{echo "(get)"!;data.(n)}
set dset [n,v]:{echo "(set)"!;data.(n)=v;TRUE}
set dcount  []:{echo "(count)"!;len data!}

set dyn         dynenv dgetall dcount dset dget!
set dyn_nocount dynenv dgetall NULL  dset  dget!
set dyn_noset   dynenv dgetall dcount NULL dget!
set dyn_noget   dynenv dgetall dcount dset NULL!

eval dyn
len dyn
set dyn.ads 77
set dyn.b 88
eval dyn.ads
eval dyn.b
eval dyn

eval dyn_nocount
len dyn_nocount
set dyn_nocount.ads 8
set dyn_nocount.b 100
eval dyn_nocount.ads
eval dyn_nocount.b
eval dyn_nocount

eval dyn_noset
len dyn_noset
set dyn_noset.ads 10
set dyn_noset.b 120
eval dyn_noset.ads
eval dyn_noset.b
eval dyn_noset

eval dyn_noget
len dyn_noget
set dyn_noget.ads 33
set dyn_noget.b 44
eval dyn_noget.ads
eval dyn_noget.b
eval dyn_noget


