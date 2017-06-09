io write io.err "Wrong!\n"
# 7
set error[rc, msg]: {
    io.write io.err ""+(str rc!)+" "+(msg)+"\n"!
}
error 25 "a really bad thing has happened"
#25 a really bad thing has happened
#35
