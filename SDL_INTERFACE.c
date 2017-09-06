#include <assert.h>

#include "SDL_INTERFACE.h"

static SDL_INTERFACE_manager_t* sdl_manager = NULL; // a singleton global sdl manager

MANAGER_agent_play_command_t _SDL_INTERFACE_prompt_play(const GAME_board_t* p_board)
{
    assert(sdl_manager != NULL && sdl_manager->state == SDL_INTERFACE_STATE_GAME); // make sure init was called and game started
    
    MANAGER_agent_play_command_t command;
    SDL_Event event;

    command.type = MANAGER_PLAY_COMMAND_TYPE_NONE;

    do // handle events until we find a command to send
    {
        SDL_GAME_WINDOW_draw_view(sdl_manager->game_window, p_board);
        SDL_WaitEvent(&event);
        command = SDL_GAME_WINDOW_handle_event(sdl_manager->game_window, &event, p_board);

    } while (command.type == MANAGER_PLAY_COMMAND_TYPE_NONE);

    return command; // return the command
}

void _SDL_INTERFACE_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response)
{
    if (command.type == MANAGER_PLAY_COMMAND_TYPE_GET_MOVES && response.has_output)
    {
        sdl_manager->game_window->marked_moves = response.output.get_moves_data.moves;
        sdl_manager->game_window->fixed_castle = FALSE;
    }
}

MANAGER_agent_command_t _SDL_play_quit_button_cb()
{
    MANAGER_agent_command_t cmd;
    cmd.type = MANAGER_COMMAND_TYPE_PLAY_COMMAND;
    cmd.cmd.play_command.type = MANAGER_PLAY_COMMAND_TYPE_QUIT;
    return cmd;
}
        

void SDL_INTERFACE_init()
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { //SDL2 INIT
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		exit(-1);
	}

    sdl_manager = malloc(sizeof(SDL_INTERFACE_manager_t));
    sdl_manager->state = SDL_INTERFACE_STATE_GAME; //tmp, should be: SDL_INTERFACE_STATE_MAIN_MENU;

    sdl_manager->game_window = SDL_GAME_WINDOW_create_view();
    SDL_GAME_WINDOW_add_button(sdl_manager->game_window, "./graphics/quit.bmp", NULL, _SDL_play_quit_button_cb);
}

MANAGER_play_agent_t SDL_INTERFACE_get_play_agent()
{
    MANAGER_play_agent_t agent;
    agent.prompt_play_command = _SDL_INTERFACE_prompt_play;
    agent.handle_play_command_response = _SDL_INTERFACE_handle_play_command_response;
    return agent;
}

MANAGER_settings_agent_t SDL_INTERFACE_get_settings_agent();

void SDL_handle_quit()
{
    SDL_GAME_WINDOW_destroy_view(sdl_manager->game_window);
    free(sdl_manager);

	SDL_Quit();
}
