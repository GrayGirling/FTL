join "/" <"dir", "subdir", "file">
#"dir/subdir/file"
join 0 <"line1", "line2">
#"line1\0line2"
join NULL <0x41, " line", 0x320, "made from", 32, "bytes">
#"A line made from bytes"
