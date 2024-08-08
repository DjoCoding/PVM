#include "src/machine.h"

char *argv_shift(int *argc, char ***argv) {
    argc--;
    char *value = **argv;
    *argv += 1;
    return value;
}

void usage(char *program) {  
    fprintf(stderr, "Usage: %s --<flags> <file_name.tim>\n", program);
    fprintf(stderr, "Flags: disasm\n");		
    exit(1);
}

int main(int argc, char **argv) {   
    char *program = argv_shift(&argc, &argv);
    char *file_path = argv_shift(&argc, &argv);

    if (!file_path) { usage(program); }

    Machine m = machine_init(); 
    Program prog = load_prog_from_file(&m, file_path);
    machine_exec_prog(&m, prog);
    machine_clean(&m);
    
    return 0;
}