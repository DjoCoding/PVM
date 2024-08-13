#include "src/disasm.h"

int argv_end(int *argc) {
    return *argc == 0;
}

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
    return sv_ends_with(SV(filepath), SV(".pvm"));
}

char *get_output_filepath(char *filepath) {
    String_View path = SV(filepath);
    String_View pvm_extension = SV(".pvm");
    String_View pasm_extension = SV(".pasm");

    char *extension = path.content + path.count - pvm_extension.count;
    memcpy(extension, pasm_extension.content, pasm_extension.count);
    extension[pasm_extension.count] = 0;
    
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

    char *output_filepath = NULL;

    while (!argv_end(&argc)) {
        char *flag = argv_shift(&argc, &argv);
        if (strcmp(flag, "-o") == 0) {
            if (argv_end(&argc)) {
                THROW_ERROR("expected output file path but end of arguments found");
            }
            if (output_filepath) { THROW_ERROR("output file path already provided `%s`", output_filepath); }
            output_filepath = argv_shift(&argc, &argv);
        } else { THROW_ERROR("invalid flag"); }
    }

    Program prog = machine_load_prog_from_file(filepath);

    if (!output_filepath) { output_filepath = get_output_filepath(filepath); }
    
    FILE *f = fopen(output_filepath, "w");
    if (!f) {
        usage(program);
        THROW_ERROR("could not open the file `%s` for writing", output_filepath);
    }
    depasm(f, prog);
    fclose(f);
    return 0;
}