set oldfmt int_fmt_hexbits (-1)!
eval <-10000, -100, 0, 100, 10000>
# <-10000, -100, 0, 100, 10000>
eval <-100000000, -10000, -100, 0, 100, 10000, 100000000>
# <-100000000, -10000, -100, 0, 100, 10000, 100000000>
int_fmt_hexbits 0xFFFF
# -1
eval <-100000000, -10000, -100, 0, 100, 10000, 100000000>
# <0xfffffffffa0a1f00, -10000, -100, 0, 100, 10000, 0x5f5e100>
int_fmt_hexbits 0xFF
# 0xffff
eval <-100000000, -10000, -100, 0, 100, 10000, 100000000>
# <0xfffffffffa0a1f00, 0xffffffffffffd8f0, -100, 0, 100, 0x2710, 0x5f5e100>
int_fmt_hexbits oldfmt
# 255
int_fmt_hexbits 0xFF
