#------------------------------------------
#@author	: Sniehovskyi Nikita, xsnieh00
#			: FIT VUTBR.CZ
#date		: 19.11.2022
#project	: IFJ project
#file		: Makefile
#------------------------------------------

# !!! Everything in src folder will be compiled !!!


CC = gcc
CFLAGS = --std=c11 -Wextra -Wall


EXEC = IFJ22_PHP_compiler.out

SRCDIR = src/
OBJDIR = objs/

SRCS = $(wildcard $(SRCDIR)*.c)
OBJS = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SRCS))



all: prep $(EXEC)

run: prep $(EXEC)
	./IFJ22_PHP_compiler.out

rm:
	rm -f $(OBJDIR)*.o
	rm -f IFJ22_PHP_compiler.out






prep:
	mkdir -p $(OBJDIR)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) -r -o $@ $<

