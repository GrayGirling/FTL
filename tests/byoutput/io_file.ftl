set exists[f]: { 0 != (cmp NULL (io.binfile f "r"!)!) }
exists "/tmp/gone"
#FALSE
exists "/etc/hosts"
#TRUE
parse exec "" (io.file "script" "r"!)
