#const HELLO "Hello World" end
#const NEW_LINE 10 end

jmp $main

#println:
	syscall 0
	push $NEW_LINE
	putc
	ret

#main:
	pushs $HELLO
	call $println 
	halt
