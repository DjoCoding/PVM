#const HELLO "hello world", NEW_LINE 10 end

jmp $main

#print:
	push $NEW_LINE
	pushs $HELLO
	syscall 0
	putc
	ret

#main:
	call $print
	halt
