echo "Hello World"
echo   === STARTING ===
# === STARTING ===
set option "on"
echo Option is currently $option
# Option is currently on
set follow[a]: { .next = add a 1!; echo "Next \$next"! }
follow 4
# Next 5