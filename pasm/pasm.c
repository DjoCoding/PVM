#include "src/asm.h"
#include "../src/machine.h"

char *argv_shift(int *argc, char ***argv) {
    char *value = **argv;
    *argc -= 1;
    *argv += 1;
    return value;
}

void usage(char *program) {  
    fprintf(stderr, "Usage: %s <prog.pasm>\n", program);
}

int filepath_correct(char *filepath) {
    return sv_ends_with(SV(filepath), SV(".pasm"));
}

char *get_output_filepath(char *filepath) {
    String_View path = SV(filepath);
    String_View pvm_extension = SV(".pvm");
    
    char *extension = path.content + path.count - pvm_extension.count - 1;
    memcpy(extension, pvm_extension.content, pvm_extension.count);
    extension[pvm_extension.count] = 0;
    
    return filepath;
}

int main(int argc, char **argv) {
    char *program = argv_shift(&argc, &argv);
    char *filepath = argv_shift(&argc, &argv);

    if (!filepath) { usage(program); }

    if (!filepath_correct(filepath)) {
        usage(program);
        THROW_ERROR("file `%s` should have the pasm extension `.pasm`", filepath); 
    }

    PASM pasm = pasm_init(filepath);
    compile(&pasm);
    pasm_store_prog_in_file(get_output_filepath(filepath), pasm.prog);
    
    return 0;
}