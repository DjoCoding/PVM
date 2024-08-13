; syscalls ids
#const WRITE                                 0 end 
#const READ                                  1 end 
#const ALLOC                                 2 end 
#const FREE                                  3 end 
#const OPEN                                  4 end 
#const CLOSE                                 5 end 
#const EXIT                                  6 end 

; special files
#const STDOUT                                0 end  
#const STDIN                                 1 end 
#const STDERR                                2 end

; open modes
#const READ_MODE                             0 end 
#const WRITE_MODE                            1 end 
#const APPEND_MODE                           2 end
#const READ_AND_WRITE_MODE                   3 end 
#const WRITE_AND_READ_MODE                   4 end 
#const APPEND_AND_READ_MODE                  5 end 
#const READ_BINARY_MODE                      6 end 
#const WRITE_BINARY_MODE                     7 end 
#const APPEND_BINARY_MODE                    8 end 
#const READ_AND_WRITE_BINARY_MODE            9 end 
#const WRITE_AND_READ_BINARY_MODE            10 end 
#const APPEND_AND_READ_BINARY_MODE           11 end 