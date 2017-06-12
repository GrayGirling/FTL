set base [val=1]
set copy1 base
set copy2 (new base!)
set base.val 2
eval copy1
#[val=2]
eval copy2
#[val=1]
 
set class [val=0]:{ [read=[]:{val}, write=[n]:{val=n;}] }
set rw1 class!
set rw2 class!
eval rw1.write 42!
eval rw2.read!
#42
# oops!

set class [val=0]:{ [read=[]:{val}, write=[n]:{val=n;}] }
set rw1 (new class!)!
set rw2 (new class!)!
eval rw1.write 42!
eval rw2.read!
#0
eval rw1.read!
#42
