int 42
int 0b100
int 0o700
int 0xF00
real 42.42
real 42e10
real .5
real 1.6e+4
real .6e-4
real 0xF00.8p-2
real 0x.8p2
real 0o7.77E2
real 0b101.1E11

eval 42
eval 0b100
eval 0o700
eval 0xF00
eval 42.42
eval 42e10
eval 0.5
eval .5
# fails - must begin with numeric
eval 1.6e+4
eval .6e-4
# fails - must begin with numeric
eval 0xF00.8p-2
eval 0x.8p2
# fails - must begin with numeric
eval 0o7.77E2
eval 0b101.1E11


eval 1
eval -1
eval 2.0
eval -2.0

eval 1+2
eval 1.0+2
eval 1+2.0
eval 1.0+2.0
eval 1-2
eval 1.0-2
eval 1-2.0
eval 1.0-2.0
eval 1*2
eval 1.0*2
eval 1*2.0
eval 1.0*2.0
eval 1/2
eval 1.0/2
eval 1/2.0
eval 1.0/2.0
eval 1 _rem_ 2
eval 1.0 _rem_ 2
eval 1 _rem_ 2.0
eval 1.0 _rem_ 2.0

eval 1.0/7