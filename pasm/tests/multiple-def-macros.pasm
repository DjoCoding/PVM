#macro mult_with_10 number end
    push $number
    push 10
    mul 
    end 

#macro mult_with_10 number end 
    push $number
    push 20
    mul 
    end

#main:
    mult_with_10 10
    ret

#entry: $main