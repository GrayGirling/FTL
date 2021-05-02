closure FALSE [inc=3] {<0, inc .. 10>}
# [inc=3]::{<0, inc .. 10>}
set key 1000 # shouldn't be used in key_chooser1
set key_chooser1[vals]:{(closure FALSE vals {key}!)!}
set key_chooser2[vals]:{(closure TRUE vals {key}!)!} # include external env
key_chooser1 ["lock"=8,"key"=-9,"bolt"=2]
# -9
key_chooser1 ["lock"=8,"bolt"=2]
# ftl ...: undefined symbol 'key'
# ftl ...: error in closure code body
key_chooser2 ["lock"=8,"bolt"=2]
# 1000
set init_42 closure TRUE [first=42] {[value=first]}!
init_42
# [value=42]

# Test constructors : and ::

# example values
set command code echo!             # code = cmd
set prog {prog}                    # code = body
set function code rnd!             # code = function (with arg)
set function0 code (parse.codeid)! # code = function (with no arg)
set unbound [arg]                  # unbound dir
set clos1_lg [arg]:prog            # closure with global context
set clos1_sml [arg]::prog          # closure with local context
set clos0_lg []:prog               # closure with global context (no arg)
set clos0_sml []::prog             # closure with local context (no arg)

eval [attr1=1]:[attr2=2]
eval [attr1=1]::[attr2=2]

context command::[attr=42]
context [attr=42]::command
eval (code ([attr=42]::command)!) == command
context command:[attr=42]
context [attr=42]:command
eval (code ([attr=42]:command)!) == command

context prog::[attr=42]
context [attr=42]::prog
eval (code ([attr=42]::prog)!) == prog
context prog:[attr=42]
context [attr=42]:prog
eval (code ([attr=42]:prog)!) == prog

context function::[attr=42]
context [attr=42]::function
eval (code ([attr=42]::function)!) == function
context function:[attr=42]
context [attr=42]:function
eval (code ([attr=42]:function)!) == function

context function0::[attr=42]
context [attr=42]::function0
eval (code ([attr=42]::function0)!) == function0
context function0:[attr=42]
context [attr=42]:function0
eval (code ([attr=42]:function0)!) == function0

eval (code clos1_lg!) == prog
context clos1_lg::[attr=42]
argnames clos1_lg::[attr=42]
inenv clos1_lg::[attr=42] "attr"
context [attr=42]::clos1_lg
argnames [attr=42]::clos1_lg
inenv [attr=42]::clos1_lg "attr"
eval (code ([attr=42]::clos1_lg)!) == prog
context clos1_lg:[attr=42]
argnames clos1_lg:[attr=42]
inenv clos1_lg:[attr=42] "attr"
context [attr=42]:clos1_lg
argnames [attr=42]:clos1_lg
inenv [attr=42]:clos1_lg "attr"
eval (code ([attr=42]:clos1_lg)!) == prog

eval (code clos1_lg!) == prog
context clos1_sml::[attr=42]
domain clos1_sml::[attr=42]
context [attr=42]::clos1_sml
domain [attr=42]::clos1_sml
eval (code ([attr=42]::clos1_sml)!) == prog
context clos1_sml:[attr=42]
inenv clos1_sml:[attr=42] "attr"
context [attr=42]:clos1_sml
inenv [attr=42]:clos1_sml "attr"
eval (code ([attr=42]:clos1_sml)!) == prog

eval (code clos1_sml!) == prog
context clos1_sml::[attr=42]
context [attr=42]::clos1_sml
eval (code ([attr=42]::clos1_sml)!) == prog
context clos1_sml:[attr=42]
context [attr=42]:clos1_sml
eval (code ([attr=42]:clos1_sml)!) == prog

context [a=1]::[b=2]::clos1_sml::[c=3]::[d=4]
domain [a=1]::[b=2]::clos1_sml::[c=3]::[d=4]
context [a=1]::[b=2]::clos0_sml::[c=3]::[d=4]
domain [a=1]::[b=2]::clos0_sml::[c=3]::[d=4]

context unbound::[attr=42]
context [attr=42]::unbound
context unbound:[attr=42]
context [attr=42]:unbound

argnames unbound::[attr=42]
argnames [attr=42]::unbound
argnames unbound:[attr=42]
argnames [attr=42]:unbound

argnames clos0_sml
argnames unbound::clos0_sml
argnames clos0_sml::unbound
argnames unbound:clos0_sml
argnames clos0_sml:unbound

argnames clos1_sml
argnames unbound::clos1_sml
argnames clos1_sml::unbound
argnames unbound:clos1_sml
argnames clos1_sml:unbound

argnames clos0_lg
argnames unbound::clos0_lg
argnames clos0_lg::unbound
argnames unbound:clos0_lg
argnames clos0_lg:unbound

argnames clos1_lg
argnames unbound::clos1_lg
argnames clos1_lg::unbound
argnames unbound:clos1_lg
argnames clos1_lg:unbound

# argument names in a function value are unknown and implicit
argnames unbound::function0
argnames function0::unbound
argnames unbound:function0
argnames function0:unbound

argnames unbound::function
argnames function::unbound
argnames unbound:function
argnames function:unbound

# argument names in a function value are meaningless
argnames unbound::command
argnames command::unbound
argnames unbound:command
argnames command:unbound


set myecho [author="Gray"]:echo
set author "Anon"

eval myecho {Author is $author}!
# should be: Author is Anon
set any echo:[author="corruption by irrelevance"]
# once caused an environment corruption error
eval myecho {Author is $author}!
# should be: Author is Anon
set any echo:[author="corruption by another irrelevance"]
eval myecho {Author is $author}!
# should be: Author is Anon

eval echo "Author is $author"!
eval []:echo {Author is $author}!
# should be: Author is Anon
eval []::echo {Author is $author}!
# should be: Author is Anon
eval echo {Author is $author}!
# should be: Author is Anon

set simple[]:{"some text"}
simple

set collect_test[]:{
   [self]:{self} "some text"!
}
collect_test
# (once displayed a garbage collection issue), should be "some text"

set myclass []:{
   [self=[]]:{
       self.state = 2;
       self
   }!
}
myclass

set myclass []:{
   [state=2]:{
       []:{state}
   }!
}
myclass
# should be []:{state}
eval myclass!!
# 2

set myclass []:{
   [self=[],state=2]:{
       self.method = []:{state};
       self
   }!
}
myclass
eval (myclass!).method!
# 2

set myclass [arg]:{
   [self=[],state=arg]:{
       enter self!;
       .method = []:{state};
       leaving!
   }!
}
myclass 2
set A myclass 2!
set B myclass 90!
eval A.method!
eval B.method!
A method
B method
