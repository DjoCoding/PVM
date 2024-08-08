CC = gcc

CFLAGS = -Wall -Werror -g

SRCDIR = src
OBJDIR = obj
TOOLSDIR = tools

ENTRY = pvmr.c

SOURCES = $(ENTRY) $(wildcard $(TOOLSDIR)/*.c)  $(wildcard $(SRCDIR)/*.c)

OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

EXEC = pvmr

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJDIR)/*.o $(EXEC)

.PHONY: all clean
