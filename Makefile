CC = gcc
CFLAGS =-std=c11 -Wall -Werror -g
LDLIBS=-lm -lncurses -lmenu
SRSC = program.c
OUTPUT =program

%: %.c
	$(CC) $(CFLAGS) program.c $(LDLIBS) -o froggy	


