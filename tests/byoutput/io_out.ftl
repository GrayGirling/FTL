io out
# $stream.EOF
set myecho[msg]: {
    io.write io.out msg!;
    io.write io.out "\n"!;
}
myecho "Time gentlemen please!"
