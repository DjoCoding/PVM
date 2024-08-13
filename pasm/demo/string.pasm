; counts the length of a string 
#macro len end
    dup 
    push 0               ;  0 initialize the length of the string 
    swap

    #start:
        dup 
        getc
        push 0
        cmp 
        jz $done
        push 1
        add 
        swap 
        push 1
        add 
        swap 
        jmp $start 

    #done:
        pop
        end 
