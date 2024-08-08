# PVM
PACKL Virtual Machine, a simple machine written in C

# Example

Below is a very simple code snippet in pasm (PACKL Assembler) that prints "Hello World" to the screen

```asm
#const HELLO "Hello World" end

jmp $main

#main:
	push_str $HELLO
	syscall 0
	push 10
	putc 
	halt
```