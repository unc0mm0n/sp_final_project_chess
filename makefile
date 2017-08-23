CC=gcc
O_FILES=PIECE.o GAME.o MANAGER.o SETTINGS.o
C_FLAGS=-std=c99 -Wall -Wextra -g

all: chessprog

chessprog: $(O_FILES)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $<

.PHONY: clean

clean:
	rm -rf *.o chessprog
