# Note - it isn't possible to test rnd predictably without having rndseed work
# This test is not repeatable over multiple operating systems
rndseed 42424242
echo ${rnd 256!}.${rnd 256!}.${rnd 256!}.${rnd 256!}
# 4.62.35.205
set choose[vec]:{ vec.(rnd (len vec!)!) }
choose <"red", "white", "blue">
# "blue"
