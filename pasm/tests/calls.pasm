#entry: $main


#main:	
	pushs "djaoued"
	call $println
	ret

#println:
	syscall 0
	push 10
	putc
	ret
