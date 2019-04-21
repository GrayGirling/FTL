set list [cmds]:{ 
   forwhile cmds [cmd]:{
       if cmd != "stop" {echo cmd!}{FALSE}!
   }!
}
list <>
# <nothing>
list <"add","subtract","print">
#add
#subtract
#print
#TRUE
list <"add","subtract","stop","ignore","ignore">
#add
#subtract
#FALSE
