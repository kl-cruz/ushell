CFLAGS = -g -std=c99 -Wall -pedantic -Wunused
LDFLAGS = -g
CC = gcc

all: test
	./ushell
main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

ushell.o: ushell.c
	$(CC) $(CFLAGS) -c $< -o $@

test: ushell.o main.o
	$(CC) $(CFLAGS) ushell.o main.o -lcurses -lncurses -o ushell

clean:
	rm -f *.o
	rm -f ushell
