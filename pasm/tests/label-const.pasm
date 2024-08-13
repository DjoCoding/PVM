#LABEL:
    halt

#const LABEL "this is an error" end

#main:
    pushs $LABEL    
    syscall 0
    halt

#entry: main
