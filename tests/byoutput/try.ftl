set try[fn]:{
    catch [ex]:{
        io.fprintf io.err "Exception: ** %v **\n"<ex>!; "caught"
    } fn!
}

try { echo "hi"!; "good"}

try { echo "hi"!; throw "AN EXCEPTION"!; echo "here - never"!; "never"}

set deep[]:{
    echo "Deep..."!;
    deeper {throw "from deeper"!}!
}

set deeper[fn]:{
    echo "Deeper..."!;
    fn!;
    echo "Baad"!;
    "bad"
}

try { echo "off we go"!; deep!; echo "Nooooo"!; "aweful" }

set intry[fn]:{
    echo "Inner try..."!;
    catch [ex]:{ echo "Caught inside"!;
                 throw (strf "wrap (%v)"<ex>!)!;
               } fn!;
    echo "Oh dear"!;
    "in problem"
}

try { echo "off we go again"!;
      deeper {intry {throw "from wayback"!}!}!;
      echo "Nooooo"!;
      "aweful"
}

#set exception []
set new_except[name, env]:{throw (env::[name=name])!}
#set exception.sig new_except "signal" [signo]

#set exception.sig (closure TRUE [signo] {
#                        throw ([signo=signo]::[name="signal"])!} !)
#set exception.sig [signo]:{throw ([signo=signo]::[name="signal"])!}


try {exception.signal 3!}
