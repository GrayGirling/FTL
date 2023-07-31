intseq 1 1 10000
# <1 .. 10000>
for (intseq 1 1 5!) [i]:{echo "value $i"!}
# value 1
# value 2
# value 3
# value 4
# value 5
for (intseq (-1) (-1) (-5)!) [i]:{echo "value $i"!}
# value -1
# value -2
# value -3
# value -4
# value -5
for (intseq 1 2 5!) [i]:{echo "value $i"!}
# value 1
# value 3
# value 5
for (intseq 0 3 5!) [i]:{echo "value $i"!}
# value 0
# value 3
intseq 0 NULL 10000
# <0 .. 10000>
intseq 20000 NULL 10000
# <20000 .. 10000>
intseq 0 10 10000
# <0, 10 .. 10000>
intseq NULL 10 10000
# <1, 11 .. 10000>
intseq NULL NULL 10000
# <1 .. 10000>
eval NULL == (intseq 0 0 5!)
# TRUE
eval NULL == (intseq 5 0 5!)
# FALSE
