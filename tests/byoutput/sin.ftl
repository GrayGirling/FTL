set digits9[x]:{round x*1000000000!}

sin 0
digits9 (sin PI/2!)
digits9 (sin PI/3!)**2
digits9 (sin PI/4!)**2

asin (sin 0!)
asin (sin PI/2!)
digits9 (asin (sin 0.3456!)!)

set one[x]:{(sin x!)**2 + (sin (PI/2 - x)!)**2}
digits9 (one 0.0000002!)
digits9 (one 0.7!)
digits9 (one (-0.7)!)
digits9 (one 100000!)
digits9 (one (-100000)!)
digits9 (one 100000000!)
# run out of resolution? - not 1000000000?

set INF 1.0/0
sin INF

set NAN INF-INF
sin NAN
