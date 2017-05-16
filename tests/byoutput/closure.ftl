closure FALSE [inc=3] {<0, inc .. 10>}
# [inc=3]::{<0, inc .. 10>}
set key_chooser[vals]:{(closure FALSE vals {key}!)!}
key_chooser ["lock"=8,"key"=-9,"bolt"=2]
# -9
key_chooser ["lock"=8,"bolt"=2]
# ftl $*console*:28: undefined symbol 'key'
# ftl $*console*:28: error in closure code body
set init_42 closure TRUE [first=42] {[value=first]}!
init_42
