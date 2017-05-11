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
