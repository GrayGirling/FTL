#!/usr/bin/env ftl

# set f io.file "out" "w"!

# to use this choose another terminal supporting netcat and use
#    nc -l 9999 | more
#

set line1  join NULL (zip <0..7> <"0123456789">!)!
set line2  join NULL (zip <0..7> <"9876543210">!)!
set line3  join NULL (zip <0..7> <"----------">!)!
set block join NULL (zip <0..13> <line1, line2, line3>!)!

set printf io.fprintf io.out

# scratchubuntu 134.27.116.48

set wrf[data=block,addr="134.27.116.122:9999"]:{
    .n = 1;
    .rdy = NULL; .lastrdy=NULL;
    .f = io.connect "tcp" addr "w"!;
    f != NULL {
        do {
            do {
                lastrdy = rdy;
                rdy = io.ready f!;
                rdy != lastrdy {printf "%d: ready %v\n" <n, rdy>!;}!;
            }{not rdy}!;
            s=io.write f data!;
            not s {printf "%d: write failed\n" <n>!;}!;
            n=n+1;
        }{rdy}!;
        io.close f!;
    }!;
}


# io write f block; io write f block; io write f block; io ready f

# disappointly still returns TRUE on Mac
