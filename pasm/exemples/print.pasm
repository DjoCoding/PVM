#const NEW_LINE_CHAR 10 end

#println:
	syscall 0
	push $NEW_LINE_CHAR
	putc 
	ret

