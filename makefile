CC=gcc
H_FILES=DEFS.h PIECE.h # all files which are only headers will trigger compilation for everything on change.
O_FILES=GAME.o FILES.o MANAGER.o CLI.o SETTINGS.o AI.o HEAP.o SDL_UTILS.o SDL_BUTTON.o SDL_SETTINGS_WINDOW.o\
	   	SDL_MAIN_WINDOW.o SDL_LOAD_WINDOW.o SDL_GAME_WINDOW.o SDL_INTERFACE.o main.o
C_FLAGS=-std=c99 -Wall -Wextra -g

all: chessprog

chessprog: $(O_FILES)
	$(CC) -o $@ $(C_FLAGS) $^ -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main

%.o: %.c %.h $(H_FILES)
	$(CC) -c $< $(C_FLAGS) -I/usr/include/SDL2 -D_REENTRANT
#	$(CC) -c $< $(C_FLAGS) -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT


main.o: main.c $(H_FILES)
	$(CC) -c $< $(C_FLAGS) -I/usr/include/SDL2 -D_REENTRANT
#	$(CC) -c $< $(C_FLAGS) -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
.PHONY: clean

clean:
	rm -rf *.o chessprog
