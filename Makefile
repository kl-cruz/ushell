CFLAGS = -g -Iformat/src -Iformat/lib -std=c99 -Wall -pedantic -Wunused
#-DUSHELL_DISABLE_ECHO
#-DUSHELL_DISABLE_MAX_ARGS_INFO
LDFLAGS = -g
CC = gcc

all: test
	./ushell
main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

ushell.o: ushell.c
	$(CC) $(CFLAGS) -c $< -o $@

format.o: format/src/format.c
	$(CC) $(CFLAGS) -c $< -o $@

printf.o: format/lib/printf.c
	$(CC) $(CFLAGS) -c $< -o $@

test: ushell.o format.o printf.o main.o
	$(CC) $(CFLAGS) ushell.o format.o printf.o main.o -lcurses -lncurses -o ushell

clean:
	rm *.o
	rm ushell
