#include "src/machine.h"

char *argv_shift(int *argc, char ***argv) {
    argc--;
    char *value = **argv;
    *argv += 1;
    return value;
}

void usage(char *program) {  
    fprintf(stderr, "Usage: %s --<flags> <path.pvm>\n", program);
}

int filepath_correct(char *filepath) {
    return sv_ends_with(SV(filepath), SV(".pvm"));
}

int main(int argc, char **argv) {   
    char *program = argv_shift(&argc, &argv);
    char *filepath = argv_shift(&argc, &argv);

    if (!filepath) { usage(program); }

    Machine m = machine_init(); 
    Program prog = machine_load_prog_from_file(filepath);
    machine_exec_prog(&m, prog);

    return 0;
}