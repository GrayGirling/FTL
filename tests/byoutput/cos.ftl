set digits9[x]:{round x*1000000000!}

cos 0
digits9 (cos PI/3!)
digits9 (cos PI/2!)
digits9 (cos PI/4!)**2

acos (cos 0!)
acos (cos PI/2!)
digits9 (acos (cos 0.3456!)!)

set one[x]:{(cos x!)**2 + (cos (PI/2 - x)!)**2}
digits9 (one PI/2!)
digits9 (one PI/2 - 0.0000002!)
digits9 (one 0.7!)
digits9 (one (-0.7)!)
digits9 (one 100000!)
digits9 (one (-100000)!)
digits9 (one 100000000!)
# run out of resolution? - not 1000000000?

set INF 1.0/0
cos INF

set NAN INF-INF
cos NAN

