#{   if (equal var NULL!) {
#        echo "nothing there"!
#    }{  do_something_with var!
#    }!
#}

eval (3 == 3) {echo "hi"!}!
#hi
#FALSE

equal 0 0
equal NULL NULL
equal "string" "string"
equal (mac {02:99:55:AB:9D:12}!) (mac {02:99:55:AB:9D:12}!)
equal (ip {192.168.0.20}!) (ip {192.168.0.20}!)
equal basetype.int basetype.int

equal 0 1
equal NULL FALSE
equal "string" "otherstring"
equal (mac {02:99:55:AB:9D:12}!) (mac {03:99:55:AB:9D:12}!)
equal (ip {192.168.0.20}!) (ip {192.168.0.30}!) 
equal basetype.int basetype.mac

equal <> <>
equal [] []

set friends_a [henry=TRUE, sophie=FALSE, antoine=TRUE, eric=TRUE, erica=FALSE]
set friends_b [henry=TRUE, sophie=FALSE, antoine=TRUE, eric=TRUE, erica=FALSE]
set friends_c new friends_a!

equal friends_a friends_a
equal friends_a friends_b
equal friends_a friends_c
