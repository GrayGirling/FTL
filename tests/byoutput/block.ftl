# tests for interpretation of {...}

eval {}
eval {}!
eval []:{}!
eval {;}!
eval []:{;}!
eval ({}!) == NULL
eval ([]:{}!) == NULL
eval ({;}!) == NULL
eval ([]:{;}!) == NULL
