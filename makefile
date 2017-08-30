CC=gcc
H_FILES=DEFS.h PIECE.h # all files which are only headers will trigger compilation for everything on change.
O_FILES=GAME.o MANAGER.o CLI.o SETTINGS.o AI.o main.o
C_FLAGS=-std=c99 -Wall -Wextra -g

all: chessprog

chessprog: $(O_FILES)
	$(CC) -o $@ $(C_FLAGS) $^

%.o: %.c %.h $(H_FILES)
	$(CC) -c $< $(C_FLAGS)

.PHONY: clean

clean:
	rm -rf *.o chessprog
