#include <assert.h>

#include "SDL_INTERFACE.h"

static SDL_INTERFACE_manager_t* sdl_manager = NULL; // a singleton global sdl manager


MANAGER_agent_command_t _SDL_MAIN_WINDOW_new_game_button_cb();
MANAGER_agent_command_t _SDL_MAIN_WINDOW_quit_button_cb();
MANAGER_agent_command_t _SDL_MAIN_WINDOW_load_game_button_cb();

MANAGER_agent_command_t _SDL_SETTINGS_WINDOW_back_button_cb()
{ 
    MANAGER_agent_command_t cmd;
    cmd.type = MANAGER_COMMAND_TYPE_SETTINGS_COMMAND;
    cmd.cmd.settings_command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;
    sdl_manager->state = SDL_INTERFACE_STATE_MAIN_MENU;
    sdl_manager->main_window = SDL_MAIN_WINDOW_create_view();

    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/newgame.bmp", NULL, _SDL_MAIN_WINDOW_new_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/loadgame.bmp", NULL, _SDL_MAIN_WINDOW_load_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/quit.bmp", NULL, _SDL_MAIN_WINDOW_quit_button_cb); 

    SDL_SETTINGS_WINDOW_destroy_view(sdl_manager->settings_window);
    sdl_manager->settings_window = NULL;
    return cmd;
}

MANAGER_agent_command_t _SDL_SETTINGS_WINDOW_start_button_cb()
{ 
    MANAGER_agent_command_t cmd;
    cmd.type = MANAGER_COMMAND_TYPE_SETTINGS_COMMAND;
    cmd.cmd.settings_command.type = MANAGER_SETTINGS_COMMAND_TYPE_START_GAME;
    sdl_manager->state = SDL_INTERFACE_STATE_GAME;
    sdl_manager->game_window = SDL_GAME_WINDOW_create_view();

    SDL_SETTINGS_WINDOW_destroy_view(sdl_manager->settings_window);
    sdl_manager->settings_window = NULL;
    return cmd;
}
MANAGER_agent_command_t _SDL_MAIN_WINDOW_new_game_button_cb()
{
    MANAGER_agent_command_t cmd;
    cmd.type = MANAGER_COMMAND_TYPE_SETTINGS_COMMAND;
    cmd.cmd.settings_command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;
    sdl_manager->state = SDL_INTERFACE_STATE_SETTINGS;
    sdl_manager->settings_window = SDL_SETTINGS_WINDOW_create_view();

    SDL_SETTINGS_WINDOW_add_back_button(sdl_manager->settings_window, "./graphics/cancel.bmp", _SDL_SETTINGS_WINDOW_back_button_cb);
    SDL_SETTINGS_WINDOW_add_start_button(sdl_manager->settings_window, "./graphics/newgame.bmp", _SDL_SETTINGS_WINDOW_start_button_cb);

    SDL_MAIN_WINDOW_destroy_view(sdl_manager->main_window);
    sdl_manager->main_window = NULL;
    return cmd;
}

MANAGER_agent_command_t _SDL_MAIN_WINDOW_load_game_button_cb()
{
    MANAGER_agent_command_t cmd;
    cmd.type = MANAGER_COMMAND_TYPE_INVALID;
    sdl_manager->state = SDL_INTERFACE_STATE_LOAD;
    //sdl_manager->load_window = SDL_LOAD_WINDOW_create_view();
    
    SDL_MAIN_WINDOW_destroy_view(sdl_manager->main_window);
    sdl_manager->main_window = NULL;
    return cmd;
}

MANAGER_agent_command_t _SDL_MAIN_WINDOW_quit_button_cb()
{
    MANAGER_agent_command_t cmd;
    cmd.type = MANAGER_COMMAND_TYPE_SETTINGS_COMMAND;
    cmd.cmd.settings_command.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT;
    return cmd;
}

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

MANAGER_agent_settings_command_t _SDL_INTERFACE_prompt_settings(const SETTINGS_settings_t* p_settings)
{
    MANAGER_agent_settings_command_t command;
    SDL_Event event;

    command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;

    do // handle events until we find a command to send
    {

    switch (sdl_manager->state)
    {
    case SDL_INTERFACE_STATE_MAIN_MENU:
    {
                
        SDL_MAIN_WINDOW_draw_view(sdl_manager->main_window);
        SDL_WaitEvent(&event);
        command = SDL_MAIN_WINDOW_handle_event(sdl_manager->main_window, &event);
        break;
    }
    case SDL_INTERFACE_STATE_SETTINGS:
    {
        SDL_SETTINGS_WINDOW_draw_view(sdl_manager->settings_window, p_settings);
        SDL_WaitEvent(&event);
        command = SDL_SETTINGS_WINDOW_handle_event(sdl_manager->settings_window, &event);
        break;
    }
    default:
    {
        assert(0);
        break;
    }
    }
    } while (command.type == MANAGER_SETTINGS_COMMAND_TYPE_NONE);

    return command; // return the command
}

void _SDL_INTERFACE_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response)
{
    if (command.type == MANAGER_PLAY_COMMAND_TYPE_GET_MOVES && response.has_output)
    {
        sdl_manager->game_window->marked_moves = response.output.get_moves_data.moves;
        sdl_manager->game_window->fixed_castle = FALSE;
    }
    else if (command.type == MANAGER_PLAY_COMMAND_TYPE_RESET)
    {
    
    sdl_manager->main_window = SDL_MAIN_WINDOW_create_view();

    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/newgame.bmp", NULL, _SDL_MAIN_WINDOW_new_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/loadgame.bmp", NULL, _SDL_MAIN_WINDOW_load_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/quit.bmp", NULL, _SDL_MAIN_WINDOW_quit_button_cb); 

    SDL_GAME_WINDOW_destroy_view(sdl_manager->game_window);
    sdl_manager->game_window = NULL;

    sdl_manager->state = SDL_INTERFACE_STATE_MAIN_MENU;
    }
}

        
void _SDL_INTERFACE_handle_settings_command_response(MANAGER_agent_settings_command_t command, MANAGER_agent_settings_command_response_t response)
{
}

void SDL_INTERFACE_init()
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { //SDL2 INIT
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		exit(-1);
	}

    sdl_manager = malloc(sizeof(SDL_INTERFACE_manager_t));
    sdl_manager->state = SDL_INTERFACE_STATE_MAIN_MENU; //tmp, should be: SDL_INTERFACE_STATE_MAIN_MENU;

    sdl_manager->game_window = NULL;

    sdl_manager->settings_window = NULL;

    sdl_manager->main_window = SDL_MAIN_WINDOW_create_view();

    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/newgame.bmp", NULL, _SDL_MAIN_WINDOW_new_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/loadgame.bmp", NULL, _SDL_MAIN_WINDOW_load_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(sdl_manager->main_window, "./graphics/quit.bmp", NULL, _SDL_MAIN_WINDOW_quit_button_cb); 
}

MANAGER_play_agent_t SDL_INTERFACE_get_play_agent()
{
    MANAGER_play_agent_t agent;
    agent.prompt_play_command = _SDL_INTERFACE_prompt_play;
    agent.handle_play_command_response = _SDL_INTERFACE_handle_play_command_response;
    return agent;
}

MANAGER_settings_agent_t SDL_INTERFACE_get_settings_agent()
{
    MANAGER_settings_agent_t agent;
    agent.get_play_agent = SDL_INTERFACE_get_play_agent;
    agent.prompt_settings_command = _SDL_INTERFACE_prompt_settings;
    agent.handle_settigns_command_response = _SDL_INTERFACE_handle_settings_command_response;
    return agent;
}

void SDL_handle_quit()
{
    SDL_GAME_WINDOW_destroy_view(sdl_manager->game_window);
    free(sdl_manager);

	SDL_Quit();
}
