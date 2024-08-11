# PVM
PACKL Virtual Machine, a simple machine written in C

# Example

Below is a very simple code snippet in pasm (PACKL Assembler) that prints "Hello World" to the screen

```asm
; A SIMPLE HELLO WORLD PROGRAM

#use "std.pasm"  ; import the standard pasm library 

#const HELLO "Hello World" end

#main:
 pushs $HELLO    ; push the constant `HELLO`
 call $println   ; call `println` subroutine defined in "std.pasm"
 ret             ; return to OS

#entry: $main    ; set `main` as entry point for the program
```
