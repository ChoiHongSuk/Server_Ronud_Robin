PROGRAM = http_main

INCLUDES = -I.

CFLAGS = $(INCLUDES) $(EXTRA_CFLAGS)

CC = gcc -Wstrict-prototypes -Wall -O2 -g

LIB =

MAKE = make

.c.o:
	    $(CC) $(CFLAGS) -c $*.c

SRC = http_main.c http_parsing.c http_main_start.c parsing_list.c http_buffer_alloc.c mem_string.c rule_parsing.c compare.c

OBJ = $(SRC:.c=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	    $(CC) $(DEFINES) $(CFLAGS) -o $@ $(OBJ) $(LDIR) $(LIB)

install:

clean:
	    rm -rf $(PROGRAM) $(OBJ)
