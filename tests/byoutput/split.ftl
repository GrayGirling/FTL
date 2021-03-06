
split "/" "dir/subdir/file"
# <"dir", "subdir", "file">

split "\0" "line1\0line2"
# <"line1", "line2">

split "" "£€¢"
# <"\xc2\xa3", "\xe2\x82\xac", "\xc2\xa2">

split NULL "£€¢"
# <"\xc2", "\xa3", "\xe2", "\x82", "\xac", "\xc2", "\xa2">

split NULL "ascii"
# <"a", "s", "c", "i", "i">

set spaceout[s]: { join " - " (split "" s!)! }
echo ${spaceout "£€¢"!}
# £ - € - ¢

set args[str]: { range (select (notequal "") (split " " str!)!)! }
args " a   badly  spaced     command line"
# <"a", "badly", "spaced", "command", "line">

set vec split "" "Test!"!
eval vec.4
#"!"
eval vec.4 == "!"
#TRUE

set vec split NULL "Test!"!
eval vec.4
#"!"
eval vec.4 == "!"
#TRUE
