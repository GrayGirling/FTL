io fprintf io.out "€1 = £%[pounds]d.%[pennies]02d\n" [pennies=60, pounds=0]
#€1 = £0.60
#14
set printf io.fprintf io.out
set n printf "sequence %02d %02d %02d %02d\n" <5..8>!
#sequence 05 06 07 08
eval n
#21
