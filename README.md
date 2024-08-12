# PVM
PACKL Virtual Machine, a simple machine written in C

# Example

Below is a very simple code snippet in pasm (PACKL Assembler) that prints "Hello World" to the screen

```asm
; A SIMPLE HELLO WORLD PROGRAM

#use "std.pasm"  ; importing a library in PASM, it contains the `print` macro

#const HELLO_WORLD "Hello World" end ; defintion of a constant in PASM

#main:
 println $HELLO  ; the `println` macro is capable of printing strings to the stdout
 ret             ; return to OS

#entry: $main    ; set `main` as entry point for the program
```
