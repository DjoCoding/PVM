#include "machine-memory.h"

size_t machine_memory_allocate(Machine *self, size_t size) {
    Memory_Cell cell;
    cell.size = size;
    cell.data = (char *)malloc(sizeof(char) * cell.size);
    if (!cell.data) { THROW_ERROR("failed to allocate memory"); }
    DA_APPEND(&self->memory, cell);
    return self->memory.count - 1;
}
