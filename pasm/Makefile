CC = gcc

CFLAGS = -Wall -Werror -g -Wextra

SRCDIR = src
TOOLSDIR = ../tools
OBJDIR = obj
MACHINEDIR = ../src

ENTRY = pasm.c

SOURCES = $(ENTRY) $(wildcard $(SRCDIR)/*.c) $(wildcard $(TOOLSDIR)/*.c) $(wildcard $(MACHINEDIR)/*.c) 

OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

EXEC = pasm

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJDIR)/*.o $(EXEC)

.PHONY: all clean
