#const BEGIN "begin const", END "end const" end

jmp $main

#main:
	push 10
	pushs $BEGIN
	syscall 0
	putc
	push 10
	pushs $END
	syscall 0
	putc 
	jmp $done

#done:
	halt
