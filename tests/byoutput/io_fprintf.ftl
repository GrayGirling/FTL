io fprintf io.out "€1 = £%[pounds]d.%[p]02d\n" [p=60, pounds=0]
#€1 = £0.60
#14
set printf io.fprintf io.out
set n printf "sequence %02d %02d %02d %02d\n" <5..8>!
#sequence 05 06 07 08
eval n
#21

fmt d 16 10 (16)
# "+0000000016"
set fmt.w [flag, prec, day]:{<"mon","tue","wed","thu","fri","sat","sun">.(day _rem_ 7)}
strf "after %w it will be %w" <3, 3+1>
# "after fri it will be sat"

fmt help

