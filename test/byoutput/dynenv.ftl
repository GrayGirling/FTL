set data [ads=3,b=9]
set dgetall []:{echo "(getall)"!;data}
set dget [n]:{echo "(get)"!;data.(n)}
set dset [n,v]:{echo "(set)"!;data.(n)=v;TRUE}
set dcount []:{echo "(count)"!;len data!}

set dyn dynenv dgetall dcount dset dget!

set dyn2 dynenv dgetall NULL dset dget!
set dynr dynenv dgetall dcount NULL dget!
set dynw dynenv dgetall dcount dset NULL!

eval dyn
len dyn
set dyn2.ads 77
set dyn2.b 88
eval dyn2.ads
eval dyn2.b
eval dyn2

eval dyn2
len dyn2
set dyn2.ads 8
set dyn2.b 100
eval dyn2.ads
eval dyn2.b
eval dyn2

eval dynr
len dynr
set dynr.ads 10
set dynr.b 120
eval dynr.ads
eval dynr.b
eval dynr

eval dynw
len dynw
set dynw.ads 33
set dynw.b 44
eval dynw.ads
eval dynw.b
eval dynw


