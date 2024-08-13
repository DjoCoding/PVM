#use "./std.pasm"

; open a file in the write mode
#macro open_file filepath mode end 
    pushs $filepath                   ; push the given filepath to the stack 
    push $mode                        ; push the opening file mode 
    syscall $OPEN                     ; call the system to open it 

    ; check for errors

    dup                               ; duplicate the file pointer
    push 0                            ; push NULL
    cmp                               ; compare with NULL
    jz $null_open                     ; if the file pointer is NULL then print the error
    jmp $return                       ; else return from the macro

    #null_open:
        pop
        println "could not open the file"
        exit 1 

    #return:
    end 

; write a string to a file, the file pointer should be on the tos
#macro write_string_to_file string size end     
    dup                             ; duplicate the file pointer 
    push $size                      ; push the size of the data we wanna write 
    pushs $string                   ; push the data itself
    syscall $WRITE                  ; call the system to write it to the file
    end 

; close the file, it should be on the tos
#macro close_file end 
    syscall $CLOSE
    end 

; exit from the program 
#macro exit code end 
    push $code 
    syscall $EXIT 
    end 

; main entry point
#main:    
    open_file "djo.pasm" $WRITE_MODE
    write_string_to_file "djo" 3
    close_file
    exit 0

#entry: $main 
