#use "./lib/std.pasm"

#const name "DJAOUED" end

#main:
    pushs $name 
    call $println 
    halt

#entry: $main