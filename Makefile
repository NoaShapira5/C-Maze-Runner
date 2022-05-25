# if you use Mac, change gcc to clang
CC = gcc
LD = gcc

CFLAGS  = -Wall -g
LDFLAGS =

all: maze
clean: 
	rm -f maze *.o

maze: maze.o terminal.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

