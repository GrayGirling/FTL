set milk [mon=4, tue=2, wed=4, thu=3, fri=3, sat=6, sun=0]
lock milk
set milk.mon 5
eval milk.mon
#5
set milk.feb 4
#ftl $*console*:6: can't set a value for "feb" here
set config_empty []
lock config_empty
