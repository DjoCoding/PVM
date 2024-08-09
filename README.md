# PVM
PACKL Virtual Machine, a simple machine written in C

# Example

Below is a very simple code snippet in pasm (PACKL Assembler) that prints "Hello World" to the screen

```asm
#const HELLO "Hello World" end
#const NEW_LINE 10 end

jmp $main

#main:
	pushs $HELLO
	syscall 0
	push $NEW_LINE
	putc 
	halt
```
