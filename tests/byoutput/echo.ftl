echo "Hello World"
echo   === STARTING ===
# === STARTING ===
set option "on"
echo Option is currently $option
# Option is currently on
set follow[a]: { [next = add a 1!]:{ echo "Next $next"! }! }
follow 4
# Next 5
set copy[1,2]:{ sys.run "echo cp '$1' '$2'"! }
copy "fileA" "fileB"
# cp fileA fileB
# (should be the same on both windows/posix)