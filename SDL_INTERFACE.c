#include <assert.h>

#include "SDL_INTERFACE.h"

static SDL_INTERFACE_manager_t* sdl_manager = NULL; // a singleton global sdl manager

MANAGER_agent_play_command_t _SDL_INTERFACE_prompt_play(const GAME_board_t* p_board)
{
    assert(sdl_manager != NULL && sdl_manager->state == SDL_INTERFACE_STATE_GAME); // make sure init was called and game started
    
    SDL_BUTTON_action_t act;
    SDL_Event event;

    act.action = SDL_BUTTON_ACTION_NONE;
    do // handle events until we find a command to send
    {
        SDL_GAME_WINDOW_draw_view(sdl_manager->game_window, p_board);
        SDL_WaitEvent(&event);
        act = SDL_GAME_WINDOW_handle_event(sdl_manager->game_window, &event, p_board);
        
    } while (act.action != SDL_BUTTON_ACTION_SEND_PLAY_CMD);

    return act.play_cmd; // return the command
}

MANAGER_agent_settings_command_t _SDL_INTERFACE_prompt_settings(const SETTINGS_settings_t* p_settings)
{
    SDL_BUTTON_action_t act;
    SDL_Event event;

    act.action = SDL_BUTTON_ACTION_NONE;

    do // handle events until we find a command to send
    {

    switch (sdl_manager->state)
    {
    case SDL_INTERFACE_STATE_MAIN_MENU:
    {
                
        SDL_MAIN_WINDOW_draw_view(sdl_manager->main_window);
        SDL_WaitEvent(&event);
        act = SDL_MAIN_WINDOW_handle_event(sdl_manager->main_window, &event);
        break;
    }
    case SDL_INTERFACE_STATE_SETTINGS:
    {
        SDL_SETTINGS_WINDOW_draw_view(sdl_manager->settings_window, p_settings);
        SDL_WaitEvent(&event);
        act = SDL_SETTINGS_WINDOW_handle_event(sdl_manager->settings_window, &event);
        break;
    }
    case SDL_INTERFACE_STATE_QUIT:
    {
        act.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
        act.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT;
    }
    default:
    {
        assert(0);
        break;
    }
    }
    if (act.action == SDL_BUTTON_ACTION_CHANGE_STATE)
    {
        SDL_INTERFACE_change_state(sdl_manager, act.new_state);
    }
    } while (act.action != SDL_BUTTON_ACTION_SEND_SETTINGS_CMD);

    return act.settings_cmd; // return the command
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
        SDL_INTERFACE_change_state(sdl_manager, SDL_INTERFACE_STATE_MAIN_MENU);
    }
}
        
void _SDL_INTERFACE_handle_settings_command_response(MANAGER_agent_settings_command_t command, MANAGER_agent_settings_command_response_t response)
{
    if (command.type == MANAGER_SETTINGS_COMMAND_TYPE_START_GAME)
    {
        SDL_INTERFACE_change_state(sdl_manager, SDL_INTERFACE_STATE_GAME);
    }
}

void SDL_INTERFACE_init()
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { //SDL2 INIT
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		exit(-1);
	}

    sdl_manager = malloc(sizeof(SDL_INTERFACE_manager_t));
    sdl_manager->state = SDL_INTERFACE_STATE_INVALID; //tmp, should be: SDL_INTERFACE_STATE_MAIN_MENU;

    sdl_manager->game_window = NULL;
    sdl_manager->settings_window = NULL;
    sdl_manager->main_window = NULL;

    SDL_INTERFACE_change_state(sdl_manager, SDL_INTERFACE_STATE_MAIN_MENU);
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
    SDL_INTERFACE_change_state(sdl_manager, SDL_INTERFACE_STATE_QUIT); // will terminate all windows
    free(sdl_manager);

	SDL_Quit();
}

void SDL_INTERFACE_change_state(SDL_INTERFACE_manager_t* p_manager, SDL_INTERFACE_STATE_E new_state)
{
    if (new_state == p_manager->state) // no state is changed
    {
        return;
    }

    switch (p_manager->state) // destroy current view
    {
        case SDL_INTERFACE_STATE_MAIN_MENU:
            SDL_MAIN_WINDOW_destroy_view(p_manager->main_window);
            p_manager->main_window = NULL;
            break;
        case SDL_INTERFACE_STATE_SETTINGS:
            SDL_SETTINGS_WINDOW_destroy_view(p_manager->settings_window);
            p_manager->settings_window = NULL;
            break;
        case SDL_INTERFACE_STATE_GAME:
            SDL_GAME_WINDOW_destroy_view(p_manager->game_window);
            p_manager->game_window = NULL;
            break;
    }

    p_manager->state = new_state;

    switch (new_state) // create new view
    {
        case SDL_INTERFACE_STATE_MAIN_MENU:
            p_manager->main_window = SDL_MAIN_WINDOW_create_view();
            if (p_manager->main_window == NULL) // SDL or allocation error
            {
                printf("Error: Unable to load main window\n");
                p_manager->state = SDL_INTERFACE_STATE_QUIT;            
            }
            break;
        case SDL_INTERFACE_STATE_SETTINGS:
            p_manager->settings_window = SDL_SETTINGS_WINDOW_create_view();
            if (p_manager->settings_window == NULL) // SDL or allocation error
            {
                printf("Error: Unable to load settings window\n");
                p_manager->state = SDL_INTERFACE_STATE_QUIT;            
            }
            break;
        case SDL_INTERFACE_STATE_GAME:
            p_manager->game_window = SDL_GAME_WINDOW_create_view();
            if (p_manager->game_window == NULL) // SDL or allocation error
            {
                printf("Error: Unable to load game window\n");
                p_manager->state = SDL_INTERFACE_STATE_QUIT;            
            }
            break;
    }

}
