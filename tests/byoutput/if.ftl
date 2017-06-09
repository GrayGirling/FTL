rndseed 123456
if 3 lt (rnd 6!) { echo "heads"! } { echo "tails"! }
eval echo (if 3 lt (rnd 6!) {"heads"}{"tails"}!)!
set doit[cmd]: {
    if 0 == (cmp cmd "exit"!) {
       FALSE
    }{
       eval cmd!;
       TRUE
    }!
}
doit "echo \"hi\"!"
doit "exit"
if TRUE (echo "this") (echo "that")
# this
if FALSE (echo "this") (echo "that")
# that
