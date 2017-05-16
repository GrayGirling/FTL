set c "$$a or $$b"
eval c
set t "thing"
set a "some$t"
set o "other"
set b "an$$o"
eval b
echo $a
echo $c

echo a is ${a}
echo b is ${b}
echo c is ${c}
echo a,b,c is ${a;b;c}

set fn[]:{"an$$o"}
echo ${fn!}

set d <1,3,9>
echo d is $d

set e <"$a","$$b","$c">
echo e is $e

# echo $@1 - what is this?

