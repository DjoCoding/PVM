jmp $done ; it won't execute this as the entry point is main

#main:
	push 65
	putc
	jmp $done

#done:
	halt

#entry: $main
	
