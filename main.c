#include "CLI.h"
#include "MANAGER.h"
#include "AI.h"
#include "SDL_INTERFACE.h"

int main(int argv, char * argc[])
{
    MANAGER_managed_game_t* game ;
    if (argv != 2 || argc[1][1] != 'g')
    {
        AI_set_print(TRUE);
        game = MANAGER_new_managed_game(CLI_get_settings_agent(), CLI_handle_quit);
    }
    else
    {
        game = MANAGER_new_managed_game(SDL_INTERFACE_get_settings_agent(), SDL_handle_quit);
        SDL_INTERFACE_init();
    }
    MANAGER_start_game(game);

    return 0;
}
