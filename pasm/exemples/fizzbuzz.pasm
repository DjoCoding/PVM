#const seed 100, fizz "fizz", buzz "buzz", done_ "done\n" end

jmp $main

#main:
	push $seed
	jmp $start

#start:
	jmp $try_fizz

#try_fizz:
	dup
	push 0
	cmp
	jle $done
	push 3
	indup 1 
	swap
	mod
	push 0
	cmp
	jz $print_fizz
	jmp $try_buzz

#try_buzz:
	dup
	push 5
	swap
	mod
	push 0
	cmp 
	jz $print_buzz

#print_buzz:
	pushs $buzz
	syscall 0
	push 5
	sub
	jmp $start

#print_fizz:
	pushs $fizz
	syscall 0
	push 3
	sub
	jmp $start

#done:
	pushs $done_
	syscall 0
	halt

