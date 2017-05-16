context [b=1,a]::{a+b}
# [b=1, a]
context set
# [_help="<name> <expr> - set value in environment", _1]
eval (context [b=1,a]:{a+b}!).b
# 1
eval (context [b=1,a]:{a+b}!).a
# ftl $*console*:12: index symbol undefined in parent '"a"'
# ftl $*console*:12: failed to evaluate expression
