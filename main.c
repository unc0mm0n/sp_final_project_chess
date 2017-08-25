#include "CLI.h"
#include "MANAGER.h"

int main()
{
    MANAGER_managed_game_t* game = MANAGER_new_managed_game(CLI_get_settings_agent());
    MANAGER_start_game(game);
    return 0;
}
