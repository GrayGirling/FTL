
set hdump[str]:{
    forall (split NULL str!) [h]:{
        io.write io.out (strf "%02X" h!)!;
     }!
} 

set printf io.fprintf io.out
printf "tokens %[one]s (not %[one]v) and %[two]d\n" [one="string", two=9]
#tokens string (not "string") and 9
#35

set show[fmt,val]:{
  .chrs=printf fmt <val>!; 
  printf "\n(%d characters)\n"<chrs>!;
}
show "%J" [a=10, b=<3,5,7>, c="this"]
#{
#    "a" : 10, 
#    "b" : [3, 5, 7], 
#    "c" : "this"
#}
#(57 characters)

set packet[op,dest,data]:{
   io.outstring [s]:{
       io.fprintf s "%.1b%.4b%s" <op, dest, data>!;
   }!
}
packet 2 0x12131415 "somedata"
# "\x02\x15\x14\x13\x12somedata"

fmt help
#d <f> <p> <val> - %d integer format
#u <f> <p> <val> - %u unsigned format
#o <f> <p> <val> - %u unsigned octal format
#x <f> <p> <val> - %x unsigned hex format
#X <f> <p> <val> - %X unsigned hex format
#s <f> <p> <val> - %s string format
#S <f> <p> <val> - %s zero terminated string format
#c <f> <p> <val> - %c character format
#b <f> <p> <val> - %b little endian binary format
#B <f> <p> <val> - %B big endian binary format
#v <f> <p> <val> - %v value format
#j <f> <p> <val> - %j (JSON) value format
#J <f> <p> <val> - %J (pretty JSON) value format
