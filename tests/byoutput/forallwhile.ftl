set lastlessthan[seq,max]:{
    .ix = NULL;
    forallwhile seq [val,index]:{
        val _lt_ max {
            ix = index;
            TRUE
        }!
    }!;
    ix
}
lastlessthan <14,88,142,156,188> 150
# 2
