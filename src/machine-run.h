#ifndef MACHINE_RUN_H
#define MACHINE_RUN_H

#include <string.h>

#include "machine-defs.h"
#include "machine-clean.h"
#include "machine-error.h"
#include "machine-stack.h"
#include "machine-memory.h"
#include "machine-routine.h"

void machine_exec_prog(Machine *self, Program prog);

#endif // MACHINE_RUN_H