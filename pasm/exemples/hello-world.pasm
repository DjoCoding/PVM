#const HELLO "Hello World" end

jmp $main

#main:
	push_str $HELLO
	syscall 0
	push 10
	putc 
	halt
