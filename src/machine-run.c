#include "machine-run.h"

FILE *get_file_stream(int64_t file) {
    switch(file) {
        case 0:
            return stdout;
        case 1:
            return stdin;
        case 2:
            return stderr;
        default:
            return (FILE *)file;
    }
}

void handle_nop_inst(Machine *self) {
    self->ip++;
}

void handle_halt_inst(Machine *self) {
    self->halted = true;
}

void handle_push_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_PUSH, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`push` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`push` instruction accepts a number as an operand"); }

    push(self, op.value);
    self->ip++;
}

void handle_pushs_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_PUSHS, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`push_str` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_STRING) { THROW_ERROR("`push_str` instruction accepts a string as an operand"); }

    char *string = (char *)op.value;
    size_t str_size = strlen(string);
    size_t mem_index = alloc(self, str_size);
    memcpy(self->memory.items[mem_index].data, string, str_size);

    // DA_APPEND(&self->str_stack, view);

    // push a pointer to the string
    push(self, (int64_t)self->memory.items[mem_index].data);

    // pushing the index of the string in the string stack
    // push(self, index);
    self->ip++;
}

void handle_pop_inst(Machine *self) {
    pop(self);
    self->ip++;
}

void handle_add_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    int64_t result = lhs + rhs;
    push(self, result);
    self->ip++;
}

void handle_sub_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    int64_t result = lhs - rhs;
    push(self, result);
    self->ip++;
}


void handle_mul_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    int64_t result = lhs * rhs;
    push(self, result);
    self->ip++;
}

void handle_div_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    if (rhs == 0) { THROW_ERROR("division by zero is undefined"); }
    int64_t result = lhs / rhs;
    push(self, result);
    self->ip++;
}

void handle_mod_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    if (rhs == 0) { THROW_ERROR("division by zero is undefined"); }
    int64_t result = lhs % rhs;
    push(self, result);
    self->ip++;
}

void handle_swap_inst(Machine *self) {
    int64_t top = pop(self), bottom = pop(self);
    push(self, top);
    push(self, bottom);
    self->ip++;
}

void handle_dup_inst(Machine *self) {
    int64_t top = peek(self);
    push(self, top);
    self->ip++;
}

void handle_inswap_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_INSWAP, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`inswap` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`inswap` instruction accepts a number as an operand"); }

    uint64_t pos = op.value;

    if (pos >= self->stack.count) { THROW_ERROR("`inswap` instruction failed, invalid position passed to the instruction"); }

    int64_t temp = peek(self);
    self->stack.items[self->stack.count - 1] = self->stack.items[self->stack.count - pos - 1];
    self->stack.items[self->stack.count - pos - 1] = temp;

    // machine_dump_stack(self);

    self->ip++;
}

void handle_indup_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_INDUP, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`indup` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`indup` instruction accepts a number as an operand"); }

    uint64_t pos = op.value;

    if (pos >= self->stack.count) { THROW_ERROR("`indup` instruction failed, invalid position passed to the instruction"); }

    push(self, self->stack.items[self->stack.count - pos - 1]);
    self->ip++;
}

void handle_write_syscall(Machine *self) {
    // size of the bytes: size
    // bytes to write: ptr
    // stream to write in: stream

    size_t size =  (size_t) pop(self);
    char *bytes = (char *) pop(self);
    FILE *f = get_file_stream(pop(self));

    fwrite(bytes, sizeof(char), size, f);
}

void handle_read_syscall(Machine *self) {
    // how many bytes to read: size
    // file pointer:           f

    size_t size = (size_t) pop(self);
    FILE *f = get_file_stream(pop(self));

    size_t index = alloc(self, size + 1);

    fread(self->memory.items[index].data, sizeof(char), size, f);
    self->memory.items[index].data[size] = 0;
    push(self, (int64_t)self->memory.items[index].data);
}

void handle_alloc_syscall(Machine *self) {
    int64_t top = pop(self);
    size_t size = (size_t)top;
    size_t cell_index = alloc(self, size);
    push(self, (int64_t)self->memory.items[cell_index].data);
}

void handle_free_syscall(Machine *self) {
    int64_t top = pop(self);
    void *ptr = (void *)top;
    free(ptr);
}

void handle_open_syscall(Machine *self) {
    Open_File_Mode mode_value = (Open_File_Mode) pop(self); 
    char *filepath = (char *)pop(self);

    if (mode_value >= MODES_COUNT) { THROW_ERROR("could not open the file `%s` due to the invalid mode value", filepath); }

    char *modes[] = {
        "r",
        "w",
        "a",
        "r+",
        "w+",
        "a+",
        "rb",
        "wb",
        "ab",
        "rb+",
        "wb+",
        "ab+",
    };

    FILE *f = fopen(filepath, modes[mode_value]);
    push(self, (int64_t)f);
}

void handle_close_syscall(Machine *self) {
    FILE *f = (FILE *)pop(self);
    fclose(f);
}

void handle_exit_syscall(Machine *self) {
    int64_t exit_code = pop(self);
    exit(exit_code);
}

void handle_syscall(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_SYSCALL, "could not execute the instruction");
    if (inst.ops.count != 1) { THROW_ERROR("`syscall` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];

    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`syscall` instruction accepts a number as an operand"); }
    if (op.value >= SYCALL_COUNT) { THROW_ERROR("could not make a system call due to the invalid operand passed to it %zu", (size_t)op.value); }

    switch((SysCall_ID) op.value) {
        case SYSCALL_WRITE:
            handle_write_syscall(self);
            break;
        case SYSCALL_READ:
            handle_read_syscall(self);
            break;
        case SYSCALL_ALLOC:
            handle_alloc_syscall(self);
            break;
        case SYSCALL_FREE:
            handle_free_syscall(self);
            break;
        case SYSCALL_OPEN:
            handle_open_syscall(self);
            break;
        case SYSCALL_CLOSE:
            handle_close_syscall(self);
            break;
        case SYSCALL_EXIT:
            handle_exit_syscall(self);
            break;
        default:
            ASSERT(false, "unreachable");
    }

    self->ip++;
}

void handle_jmp_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_JMP, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`jmp` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`jmp` instruction accepts a number as an operand"); }

    self->ip = op.value;
}

void handle_jz_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_JZ, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`jz` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`jz` instruction accepts a number as an operand"); }

    int64_t value = pop(self);
    if (value != 0) { self->ip++; return; }

    self->ip = op.value;
}

void handle_jle_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_JLE, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`jle` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`jle` instruction accepts a number as an operand"); }

    int64_t value = pop(self);
    if (value > 0)  { self->ip++; return; }

    self->ip = op.value;
}

void handle_jge_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_JGE, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`jge` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`jge` instruction accepts a number as an operand"); }

    int64_t value = pop(self);
    if (value < 0) { self->ip++; return; }

    self->ip = op.value;
}

void handle_jl_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_JL, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`jl` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`jl` instruction accepts a number as an operand"); }

    int64_t value = pop(self);
    if (value >= 0) { self->ip++; return; }

    self->ip = op.value;
}

void handle_jg_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_JG, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`jg` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`jg` instruction accepts a number as an operand"); }

    int64_t value = pop(self);
    if (value <= 0) { self->ip++; return; }

    self->ip = op.value;
}

void handle_putc_inst(Machine *self) {
    int64_t c = pop(self);
    FILE *f = get_file_stream(pop(self));
    putc((int)c, f);
    self->ip++;
}

void handle_call_inst(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_CALL, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`call` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`call` instruction accepts a number as an operand"); }

    machine_start_routine(self, self->ip);
    self->ip = op.value;
}

void handle_ret_inst(Machine *self) {
    // if the machine has no routines (even the entry one) then we stop executing
    if (machine_end_of_routines(self)) { 
        self->halted = true;
        return;
    }

    // end this routine and get the address of the previous routine
    size_t ip = machine_end_routine(self);

    // reassing the ip address
    self->ip = ip + 1;
}

void handle_stop_inst(Machine *self) {
    self->progs.items[self->progs.count - 1].state = PROGRAM_STATE_EXECTUED;
}

void handle_store_inst(Machine *self) {
    // how much data you wanna write : size 
    // what is this data : data
    // where to write : ptr
    
    size_t size = (size_t)pop(self); 
    char *data = (char *)pop(self);
    void *ptr = (void *)pop(self);
    
    memcpy(ptr, data, size);
    
    self->ip++;
}

void handle_strb_inst(Machine *self) {
    // data you wanna write (a char)
    // where to write
    int64_t c = pop(self);
    char *ptr = (char *)pop(self);
    
    *ptr = c;
    self->ip++;
}

void handle_loadb_inst(Machine *self) {
    // where to read 
    char *ptr = (char *)pop(self);
    
    push(self, (int64_t)(*ptr));
    self->ip++;
}


void handle_load_inst(Machine *self) {
    // how much data you wanna read : size 
    // where to read : ptr

    size_t size = (size_t)pop(self);
    void *ptr = (void *)pop(self);

    int64_t data = 0;
    memcpy(&data, ptr, size);
    push(self, data);

    self->ip++;
}

void handle_readc_inst(Machine *self) {
    FILE *f = get_file_stream(pop(self));
    int c = getc(f);
    push(self, (int64_t)c);
    self->ip++;
}

void handle_cmple_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    push(self, (int64_t)(lhs <= rhs));
    self->ip++;
}

void handle_cmpl_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    push(self, (int64_t)(lhs < rhs));
    self->ip++;
}

void handle_cmpge_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    push(self, (int64_t)(lhs >= rhs));
    self->ip++;
}

void handle_cmpg_inst(Machine *self) {
    int64_t rhs = pop(self), lhs = pop(self);
    push(self, (int64_t)(lhs > rhs));
    self->ip++;
}

void handle_ssp_inst(Machine *self) {
    int64_t top = pop(self);
    self->stack.count = (size_t)top;
    self->ip++;
}

void machine_exec_inst(Machine *self, Program_Inst prog_inst) {
    if (prog_inst.kind == PROGRAM_INST_PROGRAM) {
        prog_inst.as.prog->ret_ip = self->ip;
        machine_exec_prog(self, *prog_inst.as.prog);
        return;
    }

    Inst inst = prog_inst.as.inst;

    switch(inst.kind) {
        case INST_KIND_NOP:
            handle_nop_inst(self);
            break;
        case INST_KIND_HALT:
            handle_halt_inst(self);
            break;
        case INST_KIND_PUSH:
            handle_push_inst(self, inst);
            break;
        case INST_KIND_PUSHS:
            handle_pushs_inst(self, inst);
            break;
        case INST_KIND_POP:
            handle_pop_inst(self);
            break;
        case INST_KIND_ADD:
            handle_add_inst(self);
            break;
        case INST_KIND_CMP:
        case INST_KIND_SUB:
            handle_sub_inst(self);
            break;
        case INST_KIND_MUL: 
            handle_mul_inst(self);
            break;
        case INST_KIND_DIV:
            handle_div_inst(self);
            break;
        case INST_KIND_MOD:
            handle_mod_inst(self);
            break;
        case INST_KIND_SWAP:
            handle_swap_inst(self);
            break;
        case INST_KIND_DUP:
            handle_dup_inst(self);
            break;
        case INST_KIND_INSWAP:
            handle_inswap_inst(self, inst);
            break;
        case INST_KIND_INDUP:
            handle_indup_inst(self, inst);
            break;
        case INST_KIND_SYSCALL:
            handle_syscall(self, inst);
            break;
        case INST_KIND_JMP: 
            handle_jmp_inst(self, inst);
            break;
        case INST_KIND_JZ:
            handle_jz_inst(self, inst);
            break;
        case INST_KIND_JLE:
            handle_jle_inst(self, inst);
            break;
        case INST_KIND_JGE:
            handle_jge_inst(self, inst);
            break;
        case INST_KIND_JL:
            handle_jl_inst(self, inst);
            break;
        case INST_KIND_JG:
            handle_jg_inst(self, inst);
            break;
        case INST_KIND_PUTC:
            handle_putc_inst(self);
            break;
        case INST_KIND_CALL:
            handle_call_inst(self, inst);
            break;
        case INST_KIND_RET:
            handle_ret_inst(self);
            break;
        case INST_KIND_STOP:
            handle_stop_inst(self);
            break;
        case INST_KIND_STORE:
            handle_store_inst(self);
            break;
        case INST_KIND_LOAD:
            handle_load_inst(self);
            break;
        case INST_KIND_READC:
            handle_readc_inst(self);
            break;
        case INST_KIND_LOADB:
            handle_loadb_inst(self);
            break; 
        case INST_KIND_STRB:
            handle_strb_inst(self);
            break;
        case INST_KIND_CMPLE:
            handle_cmple_inst(self);
            break;
        case INST_KIND_CMPL:
            handle_cmpl_inst(self);
            break;
        case INST_KIND_CMPGE:
            handle_cmpge_inst(self);
            break;
        case INST_KIND_CMPG:
            handle_cmpg_inst(self);
            break;
        case INST_KIND_SSP:
            handle_ssp_inst(self);
            break;
        default:
            ASSERT(false, "unreachable");
    }
}

void machine_push_prog(Machine *self, Program prog) {
    DA_APPEND(&self->progs, prog);
}

int machine_end_of_progs(Machine *self) {
    return (self->progs.count == 0);
}

void machine_pop_prog(Machine *self) {
    self->progs.count--;
}

Program machine_get_current_executing_prog(Machine *self) {
    return (self->progs.items[self->progs.count - 1]);
} 

void machine_end_current_prog(Machine *self) {
    self->progs.items[self->progs.count - 1].state = PROGRAM_STATE_EXECTUED;
}

void machine_get_back_to_prev_prog(Machine *self) {
    self->ip = machine_get_current_executing_prog(self).ret_ip + 1;
    machine_pop_prog(self);
}

void machine_exec_prog(Machine *self, Program prog) {
    machine_push_prog(self, prog);
    self->halted = false;

    // set the entry point
    if (prog.entry.entry_set) {
        self->ip = prog.entry.ip;
    } else { self->ip = 0; }


    if (prog.count == 0) { machine_end_current_prog(self); }

    while (!self->halted) {
        if (machine_get_current_executing_prog(self).state == PROGRAM_STATE_EXECTUED) {
            if (machine_end_of_progs(self)) { self->halted = true; break; } 
            machine_get_back_to_prev_prog(self);
        } else {
            machine_exec_inst(self, machine_get_current_executing_prog(self).items[self->ip]);
        }
    }
}   