eval []
eval [z=NULL]
eval ["z"=NULL]
eval [1=NULL]
eval [a=0,b=1]
eval ["z"=NULL,"z"=4] # should have one z=4
eval [z=NULL,z=1,z=2]
eval ["z"=NULL,"z "=4]

len []
len [z=NULL]
len ["z"=NULL]
len [1=NULL]
len [a=0,b=1]
len ["z"=NULL,"z"=4]
len [z=NULL,z=1,z=2]
len ["z"=NULL,"z "=4]

eval [a]
eval [a, b]
eval [a, a]   # not a bug
eval [a=1, b]
eval [a=1, a] # bug?
eval [a, b=1] # b=1 unexpected

eval [a=1]::[b=2]
eval [d=2,e=3]::[c=1]::[a=-1,b=0]
eval [a]::[b=1]    # error
eval [a=1]::[b]    # no error
eval [a=1]::[b,c,d]
eval [a=1]::[b]::[c,d] # error - implementation failure
eval [a=1,b=2]::[a=5]

len [a=1]::[b=2]
len [d=2,e=3]::[c=1]::[a=-1,b=0]
len [a=1]::[b]    # no error
len [a=1]::[b,c,d]
len [a=1]::[b]::[c,d] # error - implementation failure
len [a=1,b=2]::[a=5]

eval [a=0]:[]
eval []:[a=0]
eval [a=0]:[b=1]
eval [b=1]:[a=0]
eval [a]:[] # error
eval []:[a]
eval [a=1]::[b=2]:[c=3] # bug - should include root

eval ([echo=4]:{}).echo   # should be 4, not global echo
eval [a=1]::[c=2]::[c=3]  # should have one c=3

eval [a] 4
eval [a=1]::[b]      2
eval [a=1]::[b,c,d]  2
eval [a=1]::[b,c,d]  2 3
eval [a=1]::[b,c,d]  2 3 4
eval [a=1]::[b,c,d]  2 3 4 5 # error
