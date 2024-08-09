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

void read_string_from_file(FILE *f, Machine *self, size_t current) {
    String_View s = {0};

    // get the length of the string
    fread(&s.count, sizeof(size_t), 1, f);

    // allocate the number of bytes needed
    s.content = malloc(sizeof(char) * s.count);

    // read the string
    fread(s.content, sizeof(char) * s.count, 1, f);

    self->str_stack.items[current] = s;
}

void read_strings_from_file(FILE *f, Machine *self) {
    for (size_t i = 0; i < self->str_stack.count; ++i) {
        read_string_from_file(f, self, i);
    }
}

void load_strings_from_file(FILE *f, Machine *self) {
    // read the number of strings
    size_t num_strings = 0;
    fread(&num_strings, sizeof(size_t), 1, f);

    // allocate the number of strings
    self->str_stack.items = malloc(sizeof(String_View) * num_strings);
    self->str_stack.count = num_strings;
    self->str_stack.size = num_strings;

    // read the strings
    read_strings_from_file(f, self);
}

void read_instruction_from_file(FILE *f, Program *prog, size_t current) {
    Inst inst = {0};

    // read the instruction kind
    fread(&inst.kind, sizeof(inst.kind), 1, f);

    // read the number of operands
    fread(&inst.ops.count, sizeof(size_t), 1, f);
    inst.ops.size = inst.ops.count;

    // allocate the memory
    inst.ops.items = malloc(sizeof(Inst_Op) * inst.ops.count);

    for (size_t i = 0; i < inst.ops.count; ++i) {
        fread(&inst.ops.items[i], sizeof(Inst_Op), 1, f);
    }

    prog->items[current] = inst;
}

void load_instructions_from_file(FILE *f, Program *prog) {
    // read the number of instructions
    fread(&prog->count, sizeof(size_t), 1, f);
    prog->size = prog->count;

    // allocate the necessary memory for the copying
    prog->items = malloc(sizeof(Inst) * prog->count);

    // read the instructions
    for (size_t i = 0; i < prog->count; ++i) {
        read_instruction_from_file(f, prog, i);
    }
}

Program load_prog_from_file(Machine *self, char *filename) {
    Program prog = {0};

    FILE *f = fopen(filename, "rb");
    if (!f) { THROW_ERROR("could not open the file %s for reading the pvm program", filename); }

    fseek(f, 0, SEEK_SET);
    

    load_strings_from_file(f, self);
    load_instructions_from_file(f, &prog);

    return prog;
}


