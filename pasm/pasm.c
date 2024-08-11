#include "src/asm.h"
#include "../src/machine.h"


char *argv_shift(int *argc, char ***argv) {
    argc--;
    char *value = **argv;
    *argv += 1;
    return value;
}

void usage(char *program) {  
    fprintf(stderr, "Usage: %s <file_name.pasm>\n", program);
    exit(1);
}

int main(int argc, char **argv) {
    char *program = argv_shift(&argc, &argv);
    char *file_path = argv_shift(&argc, &argv);

    if (!file_path) { usage(program); }

    PASM pasm = pasm_init(file_path);

    compile(&pasm);
    
    Machine machine = machine_init();
    machine_exec_prog(&machine, pasm.prog);
    machine_clean(&machine);
    
    return 0;
}