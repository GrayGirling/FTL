# indexing

set v1 <0, 1, 2, 3>

eval v1.0
eval v1.3

set v2 <0 .. 3>

eval v2.0
eval v2.3

eval <0, 1, 2, 3>.0
eval <0, 1, 2, 3>.3

eval <0..3>.0
eval <0..3>.3

# indexing errors

eval v1.-1
eval v1.4

eval v2.-1
eval v2.4

eval <0, 1, 2, 3>.-1
eval <0, 1, 2, 3>.4

eval <0..3>.-1
eval <0..3>.4


# vector index

eval v1.<0>
eval v2.<0>
eval <0, 1, 2, 3>.<0>
eval v1.<3, 2>
eval v2.<3, 2>
eval <0, 1, 2, 3>.<3, 2>

eval v1.[a=2, b=3]
eval v2..[a=2, b=3]
eval <0, 1, 2, 3>..[a=2, b=3]


# bindings

eval [a] 1
eval [b=3, a] 1
eval [b=3]::[a] 1
eval [a]::[b=3] 1
eval [a]::{} 1
eval [a]:{} 1
