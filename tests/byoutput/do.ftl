set nosix_run[]: {
    count = 0;
    do {n = rnd 6!; 
        count = 1+(count);  
        count 
    } { 0 != (n) }!
}
#nosix_run
#5
#nosix_run
#10
#nosix_run
#1