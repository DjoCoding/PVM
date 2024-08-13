#macro print string end 
    pushs $string
    syscall 0
    end

#entry: $main 

#main:
    print "Hello World"
    halt