set name "Eric"
echo Your name is $name
# Your name is Eric
echo {Your name is $name}
# {Your name is $name}
set hi[name]:{echo "Hello $name"!}
hi "sunshine"
# Hello sunshine

echo hi is $hi
# hi is [name]:{echo "Hello $name"!}

set age 50
eval echo "Hi $name, happy ${age}th"!
# Hi Eric, happy 50th
eval echo {Hi $name, happy ${age}th}!
# Hi Eric, happy 50th
set person [name="Sunny Joe", age=30]
person eval echo "Hi $name, happy ${age}th"!
# Hi Eric, happy 50th
person eval echo {Hi $name, happy ${age}th}!
# Hi Sunny Joe, happy 30th

set args <"copy", "fileA", "fileB">
echo Program ${args.0}
# Program copy
args eval echo {${.0}ing $1 to $2}!
# copying fileA to fileB
set log[0,1,2]:{ echo "${.0}ing $1 to $2"! }
log "copy" "fileA" "fileB"
# copying fileA to fileB
