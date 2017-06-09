rndseed 0x123456789
set nosix_run[]: {
    count = 0;
    do {n = rnd 6!; 
        count = 1+(count);  
        count 
    } { 0 != (n) }!
}
nosix_run
nosix_run
nosix_run
