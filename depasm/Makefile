# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -I./src -ggdb2

# Directories
SRC_DIR = src
OBJ_DIR = obj
TOOLSDIR = ../tools
MACHINEDIR = ../src
MAIN = depasm.c
BIN = ../bin

# Source Files
SRC_FILES = $(MAIN) $(wildcard $(SRC_DIR)/*.c)  $(wildcard $(TOOLSDIR)/*.c) $(wildcard $(MACHINEDIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Executable
EXEC = depasm

# Targets
.PHONY: all clean

all: $(EXEC)

# Link object files to create the executable
$(EXEC): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^
	@mkdir -p $(BIN)
	mv $(EXEC) $(BIN)

# Compile C source files to object files in a single directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

# Clean the build
clean:
	rm -f $(OBJ_DIR)/*.o $(EXEC)

