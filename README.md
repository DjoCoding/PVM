# PVM
PACKL Virtual Machine, a simple machine written in C

# Example

Below is a very simple code snippet in pasm (PACKL Assembler) that prints "Hello World" to the screen

```asm
; A SIMPLE HELLO WORLD PROGRAM

#const HELLO "Hello World" end

#println:
 syscall 0       ; call the write syscall
 push 10         ; push '\n'
 putc            ; write the char 
 ret             ; return to caller 

#main:
	pushs $HELLO    ; push the constant `HELLO`
	call $println   ; call `println`
	halt            ; halt the machine

#entry: $main    ; set `main` as entry point for the program
```
