#ifndef MACHINE_H
#define MACHINE_H

#include "machine-defs.h"
#include "machine-clean.h"
#include "machine-stack.h"
#include "machine-memory.h"
#include "machine-routine.h"
#include "machine-run.h"

Machine machine_init();
Program machine_load_prog_from_file(char *filepath);

#endif // MACHINE_H