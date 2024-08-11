#include "machine.h"

Machine machine_init() {
    Machine m;
    
    DA_INIT(&m.memory, sizeof(Memory_Cell));
    DA_INIT(&m.stack, sizeof(int64_t));
    DA_INIT(&m.str_stack, sizeof(String_View));

    m.subroutines.count = 0;
    m.halted = 1;
    m.ip = 0;

    return m;
}


