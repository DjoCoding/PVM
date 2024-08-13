#use "std.pasm"
#use "string.pasm"

#main:
    pushs "djaoued"
    len
    push $STDOUT
    inswap 2
    swap
    syscall $WRITE
    halt



#entry: $main 