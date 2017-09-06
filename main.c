#include "CLI.h"
#include "MANAGER.h"
#include "AI.h"
#include "SDL_INTERFACE.h"
#include "SDL_GAME_WINDOW.h"

int main()
{
    AI_set_print(TRUE);
    MANAGER_managed_game_t* game = MANAGER_new_managed_game(CLI_get_settings_agent(), CLI_handle_quit);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { //SDL2 INIT
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GAME_WINDOW_view_t* gw = SDL_GAME_WINDOW_create_view();
	SDL_GAME_WINDOW_draw_view(gw, game->p_board);

    MANAGER_start_game(game);

    SDL_GAME_WINDOW_destroy_view(gw);

	SDL_Quit();
    
    return 0;
}
