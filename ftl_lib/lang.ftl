# execute csdev commands instead of FTL
set prog[_help="<csdev_code> - execute csdev commands",code]:{ parse.exec NULL (io.instring code "r"!)!}

set IF [_help="<ftl_conditon> <true_csdev_code> <false_csdev_code> - if then else",
        ftl_condition, true_csdev, false_csdev]:{
   if ftl_condition {prog true_csdev!} {prog false_csdev!}!
}

set WHILE [_help="<ftl_conditon> <csdev_code> - while condition do code",
           ftl_condition, csdev_code]:{
   while ftl_condition {prog csdev_code!}!
}

set DO [_help="<csdev_code> <ftl_conditon> - do code while condition",
        csdev_code, ftl_condition]:{
   do {prog csdev_code!} ftl_condition!
}

set FOR [_help="<var> <env> <csdev_code> - run code with <var> set to each value in <env>", var, env, csdev_code]:{
   forall env [(var)]:{prog csdev_code!}!
}
