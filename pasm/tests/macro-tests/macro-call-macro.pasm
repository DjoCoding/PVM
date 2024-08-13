#macro println string end 
    print $string   
    push 10
    putc
    end

#macro print string end 
    pushs $string
    syscall 0
    end 

#main:
    println "Hello World"
    halt 

#entry: $main