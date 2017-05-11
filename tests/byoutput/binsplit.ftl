set bin "\xff\xff\xff\xff\x80\x00\x00\x01\x01\x00\x00\xff"
int_fmt_hexbits 255

binsplit TRUE TRUE 4 bin
binsplit TRUE FALSE 4 bin
binsplit FALSE TRUE 4 bin
binsplit FALSE FALSE 4 bin

binsplit TRUE TRUE 2 bin
binsplit TRUE FALSE 2 bin
binsplit FALSE TRUE 2 bin
binsplit FALSE FALSE 2 bin

binsplit TRUE TRUE 1 bin
binsplit TRUE FALSE 1 bin

