#const HELLO "Hello World" end

jmp $main

#main:
	pushs $HELLO
	syscall 0
	push 10
	putc 
	halt
