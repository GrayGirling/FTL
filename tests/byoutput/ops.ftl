# Test for the operator parsing functions in FTL

set ops <>           # op definitions with explicit priorities

parse opset ops 6  parse.assoc."fy"  "_ng_"  neg
parse opset ops 8  parse.assoc."xfy" "_sb_"  sub
parse opset ops 8  parse.assoc."yfx" "_rsb_" [x,y]:{y-x}  # reverse subtract
parse opset ops 10 parse.assoc."fy"  "_mi_"  neg

set opdef range ops! # op definitions with relative priorities

parse opeval opdef {20 _sb_ 4}
parse opeval opdef {15 _rsb_ 30}
parse opeval opdef {20 _rsb_ 15 _rsb_ 30} # 20 _rsb_ (15 _rsb_ 30)
parse opeval opdef {20 _sb_ 4 _sb_ 6}     # (20 _sb_ 4) _sb_ 6
parse opeval opdef {_ng_ 20 _sb_ 4}       # _ng_ (20 _sb_ 4)
parse opeval opdef {_mi_ 20 _sb_ 4}       # (_mi_ 20) _sb_ 4

set ops <>      # op definitions with explicit priorities
set opdef NULL  # op definitions with relative priorities

set opset[pri, as, name, fn]:{
    parse.opset ops pri parse.assoc.(as) name fn!;
    opdef = range ops!;
}

# unquoted value format
set fmt.t [f,p,v]:{ if (equal "string" (typename v!)!) {v} {str v!}!}

set diadic[strf=strf,name,l,r]::{ strf "(%s %t %t)" <name, l, r>! }
set monadic[strf=strf,name,lr]::{ strf "(%s %t)" <name, lr>! }

opset 6 "xfy" "xfy" (diadic "xfy")
opset 6 "yfx" "yfx" (diadic "yfx")


opset 6 "fy" "fy" (monadic "fy")
opset 6 "fx" "fx" (monadic "fx")
opset 6 "yfy" "yfy" (diadic "yfy")
opset 6 "xfx" "xfx" (diadic "xfx")
opset 6 "xfy" "xfy" (diadic "xfy")
opset 6 "yfx" "yfx" (diadic "yfx")
opset 6 "yf" "yf" (monadic "yf")
opset 6 "xf" "xf" (monadic "xf")
opset 8 "fy" "Fy" (monadic "Fy")
opset 8 "fx" "Fx" (monadic "Fx")
opset 8 "yfy" "yFy" (diadic "yFy")
opset 8 "xfx" "xFx" (diadic "xFx")
opset 8 "xfy" "xFy" (diadic "xFy")
opset 8 "yfx" "yFx" (diadic "yFx")
opset 8 "yf" "yF" (monadic "yF")
opset 8 "xf" "xF" (monadic "xF")

set opeval[expr]: { parse.opeval opdef expr! }

set x 42
opeval {yfy}       # fails - 'yfy' undefined
opeval {x}         # returns 42
opeval {4}
opeval {"str"}
opeval {<3>}
opeval {(4)}
opeval {5 xfy 6}
opeval {5 yfx 6}
opeval {5 yfy 6}
opeval {5 xfx 6}
opeval {fx 7}
opeval {fy 7}
opeval {4 xf}
opeval {4 yf}
opeval {5 xfy 6 xfy 7} # should give (xfy (xfy 5 6) 7)
opeval {5 yfx 6 yfx 7} # should give (yfx 5 (yfx 6 7))


set forops[opdefs, fn]:{
    forall parse.op [precfns, prec]:{
        forall precfns [precfn, name]:{
            fn name prec precfn.assoc precfn.fn!
        }!
    }!
}
set assocstr[as]:{ (domain (select (equal as) parse.assoc!)!).0 }
set printf io.fprintf io.out
forops parse.op [name, prec, assoc, fn]:{
    printf "%3d: %3s %v\n" <prec, assocstr assoc!, name>!;
}
