#main:
    println "Hello World"
    halt

#macro println string end   
        pushs $string
        syscall 0
        push 10
        putc
    end 

#entry: $main