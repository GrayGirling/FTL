#!/usr/bin/env ftl

set eprintf io.fprintf io.err

# warning - generates lots of garbage - don't run forever
set inserv[_help="<in> - demo of input polling on stream <in> - terminates on reading 'x'",
           timeout=100000, in]:{
    .haltsrv = FALSE;
    .blocked = FALSE;
    .to = timeout;
    do {
        # eprintf "."<>!;
        if (io.inblocked in!) != FALSE {
            not blocked {eprintf "Blocked\n"<>!}!;
            blocked = TRUE;
        }{
            to = timeout;
            blocked {eprintf "Unblocked\n"<>!}!;
            blocked = FALSE;
            c = io.getc in!;
            eprintf "in: %v\n"<c>!;
            c == NULL {haltsrv = TRUE}!;
            c == "x" {haltsrv = TRUE}!;
        }!;
        to = to - 1;
        to _lt_ 0 {eprintf "Input timed out\n"<>!; haltsrv=TRUE}!;
    }{not haltsrv}!
}



# warning - generates lots of garbage - don't run forever
set outserv[_help="<port> <out> - echo server polling input and serving port <port>",
            port,out]:{
    io.fprintf io.out "Wait for connection on port %d..."<port>!;
    io.flush io.out!;
    .p = io.listen "tcp" (strf "%d"<port>!) "r"!;
    io.write io.out "\n"!;
    if p != NULL {
        .ok = TRUE;
        .idle = TRUE;
        do {
            idle=TRUE;
            not (io.inblocked io.in!) != FALSE {
                eprintf "Local: " <>!;
                idle = FALSE;
                c = io.getc io.in!;
                eprintf "in: %v\n"<c>!;
            }!;
            not (io.inblocked p!) != FALSE {
                eprintf "Net: "<>!;
                .data = io.read p 100!;
                idle = FALSE;
                eprintf "read %v [%d]\n"<
                       data, if data==NULL{0}{len data!}!>!;
                ok = data != NULL {data != ""}! {
                    (io.write out data!) _gt_ 0
                }!;
            }!;
            idle { sleep 1! }!;
        } { ok }!;
        TRUE
    }{ eprintf "Failed to open TCP port %d\n"<port>!; FALSE }!
}



set remhost "127.0.0.1"
set remout NULL

set locservout[_help="<port> - start outputing to server on local host port <port>", port]:{
    remout = io.connect "tcp"  (strf "%s:%d"<remhost,port>!) "w"!;
    if remout != NULL {
       io.fprintf io.out "Opened local TCP port %d\n"<port>!; TRUE
    }{ io.fprintf io.out "Failed to open TCP port %d\n"<port>!; FALSE
    }!
}


set rprintf[_help="<fmt> <info> - formatted output to remote server", fmt, info]:{ 
    io.fprintf remout fmt info! }

set rprint[msg]:{ io.write remout ""+(msg)+"\n"! }
set recho cmd  rprint "<message> - echo message remotely"!

set rclose[_help="- close echo connection to remote host"]:{ io.close remout! }



enter [ inserv=inserv, outserv=outserv, 
        locservout=locservout, rprintf=rprintf, recho=recho, rclose=rclose ]
