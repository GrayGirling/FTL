# set codeid "something"
# set verbose FALSE - should be set by rltool itself

set printf[fmt, args]:{io.fprintf io.out fmt args!;}
set errorf[fmt, args]:{io.fprintf io.err (""+(codeid)+": "+(fmt)+"\n") args!;}
set vprintf[fmt, args]:{rl.verbose {printf fmt args!}!;}
set vecho[msg]:{rl.verbose {printf "%s\n" <msg>!;}!;}
