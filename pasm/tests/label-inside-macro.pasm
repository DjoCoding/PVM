#macro loop number end 
    push $number
    
    #loop:
        dup 
        push 0
        cmp
        jz $done
        push 1
        sub
        jmp $loop   
    #done: 
        stop
    
    end 

#main:
    loop 10
    ret

#entry: $main
