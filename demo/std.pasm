#macro println string end 
	pushs $string
	syscall 0
	push 10
	putc
	end 


