#macro println string end
    pushs $string
    syscall 0
    push 10
    putc
    end 

#macro first end
    println "djo"
    end

#macro second end 
    println "foo"
    end 

#main:
    first
    second 
    ret

#entry: $main 