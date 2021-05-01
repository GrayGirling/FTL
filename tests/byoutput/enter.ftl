set outer_fn [_help="OUTER"]:{}
enter [outer=outer_fn]
set local_fn [_help="LOCAL"]:{}

# expected to be TRUE
inenv local_fn "inenv"
inenv local_fn "outer_fn"
inenv local_fn "local_fn"
inenv local_fn "_help"
eval local_fn._help # "LOCAL"

inenv outer "inenv"
inenv outer "outer_fn"
inenv outer "_help"
eval outer._help    # "OUTER"

inenv outer_fn "inenv"
inenv outer_fn "outer_fn"
inenv outer_fn "_help"
eval outer_fn._help # "OUTER"

# expected to be FALSE
inenv outer "local_fn"
inenv outer_fn "local_fn"


set outvar 1
enter []
set outvar 100
leave
eval outvar
# should be 100

enter []
set .outvar 1000
leave
eval outvar
# should still be 100

set fn NULL
enter []
set priv 100
set fn []:{priv}
leave
fn
# should be 100
eval priv
#ftl $...: undefined symbol 'priv'


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
A method
B method

enter A
enter method
eval state
# 2
leave
eval method!
# 2
leave
