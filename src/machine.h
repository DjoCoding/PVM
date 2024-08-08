#ifndef MACHINE_H
#define MACHINE_H

#include "machine-defs.h"
#include "machine-clean.h"
#include "machine-stack.h"
#include "machine-memory.h"
#include "machine-run.h"

Machine machine_init();
Program load_prog_from_file(Machine *self, char *filename);

#endif // MACHINE_H