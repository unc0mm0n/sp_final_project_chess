#include "CLI.h"
#include "MANAGER.h"
#include "AI.h"
#include "SDL_INTERFACE.h"
#include "SDL_GAME_WINDOW.h"

int main()
{
    AI_set_print(TRUE);
    MANAGER_managed_game_t* game = MANAGER_new_managed_game(CLI_get_settings_agent(), SDL_handle_quit);
    SDL_INTERFACE_init();

    MANAGER_start_game(game);

    return 0;
}
