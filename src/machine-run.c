#include "machine-run.h"

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
    
    String_View view = SV(string);
    size_t index = self->str_stack.count;

    DA_APPEND(&self->str_stack, view);

    // pushing the index of the string in the string stack
    push(self, index);
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
    int64_t top = pop(self);
    size_t pos = (size_t)top;
    
    if (pos >= self->str_stack.count) { THROW_ERROR("invalid memory region accessed"); }

    String_View string = self->str_stack.items[pos];
    printf(SV_FMT, SV_UNWRAP(string));
}

void handle_read_syscall(Machine *self) {
    // read the number of bytes available in the top of the stack
    // allocate the string in the heap
    // push the string position to the stack

    int64_t top = pop(self);
    size_t size = (size_t)top;

    size_t index = alloc(self, size);
    for (size_t i = 0; i < size; ++i) {
        char c = getchar();
        self->memory.items[index].data[i] = c == '\n' ? 0 : c;
    }
    push(self, index);
}

void handle_syscall(Machine *self, Inst inst) {
    ASSERT(inst.kind == INST_KIND_SYSCALL, "could not execute the instruction");

    if (inst.ops.count != 1) { THROW_ERROR("`syscall` instruction accepts one operand"); }

    Inst_Op op = inst.ops.items[0];
    if (op.kind != OP_KIND_NUMBER) { THROW_ERROR("`syscall` instruction accepts a number as an operand"); }

    switch((SysCall_ID) op.kind) {
        case SYSCALL_WRITE:
            handle_write_syscall(self);
            break;
        case SYSCALL_READ:
            handle_read_syscall(self);
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
    int64_t top = pop(self);
    putchar((int)top);
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

void machine_exec_inst(Machine *self, Inst inst) {
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
        default:
            ASSERT(false, "unreachable");
    }
}

void machine_exec_prog(Machine *self, Program prog) {
    if (prog.count == 0) { return; }

    self->halted = false;
    
    // set the entry point
    self->ip = prog.entry;

    while (!self->halted) {
        if (self->ip >= prog.count) { THROW_ERROR("instruction pointer out of bounds, did you forget to add `halt`?"); }
        machine_exec_inst(self, prog.items[self->ip]);
    }
}
