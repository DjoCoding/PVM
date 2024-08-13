#use "std.pasm"

; basic print macro, the string should be on the top of the stack 
#macro println end 
	#begin:
        dup
        push 1
        gmem 
        
        dup 
        
        push 0
        cmp 
        jz $done
        
        push $STDOUT
        swap
        putc 
        
        push 1
        add 
        jmp $begin 
        
    #done:
        pop
		push $STDOUT
		push 10
		putc 
	end 

; basic read macro, the size should be on the top of the stack
#macro readln end 
    push $STDIN 
    swap 
    syscall $READ 
    end