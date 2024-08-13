#use "std.pasm"
#use "string.pasm"

#main:
    pushs "djaoued\n"
    len
    push $STDOUT
    inswap 2
    swap
    syscall $WRITE
    halt



#entry: $main 