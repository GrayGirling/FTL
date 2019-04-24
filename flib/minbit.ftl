set minbit[n]:{ n _bitand_ ((n-1) _bitxor_ n) }

set bitrev4 <0,8,4,0xc,2,0xa,6,0xe, 1,9,5,0xd,3,0xb,7,0xf>

set bitrev8[n]:{
    (bitrev4.(n _bitand_ 0xf) _shl_ 4) _bitor_
    bitrev4.((n _shr_ 4) _bitand_ 0xf)
}

set bitrev32[n]:{
    ((bitrev8 (n _bitand_ 0xff)!)            _shl_ 24) _bitor_
    ((bitrev8 ((n _shr_ 8) _bitand_ 0xff)!)  _shl_ 16) _bitor_
    ((bitrev8 ((n _shr_ 16) _bitand_ 0xff)!) _shl_ 8)  _bitor_
    (bitrev8 ((n _shr_ 24) _bitand_ 0xff)!)
}

set bitrev64[n]:{
    ((bitrev32 (n _bitand_ 0xffffffff)!) _shl_ 32) _bitor_
    (bitrev32 ((n _shr_ 32) _bitand_ 0xffffffff)!)
}

set maxbit[wfn,n]:{ wfn (minbit (wfn n!)!)! }

set maxbit (maxbit bitrev64)

