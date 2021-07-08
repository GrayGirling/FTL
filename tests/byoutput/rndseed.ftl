# This test is not repeatable over multiple operating systems

set test[]:{
    io.fprintf io.out "%d.%d.%d.%d colour " <
         rnd 256!, rnd 256!, rnd 256!, rnd 256!>!;
    echo <"red", "white", "blue">.(rnd 3!)!;
}

set myseed 56789012
echo Using seed $myseed
rndseed myseed; test
test
rndseed myseed; test

rndseed "a longer string to extract a seed from"
