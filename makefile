CC=gcc
O_FILES=GAME.o MANAGER.o CLI.o SETTINGS.o AI.o main.o
H_FILES=DEFS.h PIECE.h # all files which are only headers will trigger compilation for everything on change.
C_FLAGS=-std=c99 -Wall -Wextra -g

all: chessprog

chessprog: $(O_FILES)
	$(CC) -o $@ $^

%.o: %.c %.h $(H_FILES)
	$(CC) -c $<

.PHONY: clean

clean:
	rm -rf *.o chessprog
