#const BEGIN "begin" end

jmp $main

#main:
	pushs $BEGIN
	syscall 0
	halt
