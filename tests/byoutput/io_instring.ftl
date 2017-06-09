parse exec "" (io.instring "echo hi\necho there" "r"!)
#hi
#there
set f io.instring "text to read\n" "r"!
io read f 4
#"text"
io close f
