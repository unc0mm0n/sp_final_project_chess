#include <stdlib.h>
#include <assert.h>

#include "MANAGER.h"

/**
 * Handle a game loop iteration in the MANAGER_STATE_SETTINGS 
 * state.
 * 
 * @param p_a_manager pointer to managed game
 */
void _MANAGER_handle_settings(MANAGER_managed_game_t* p_a_manager)
{
    MANAGER_agent_settings_command_t command;
    MANAGER_agent_settings_command_response_t response;

    command = p_a_manager->settings_agent.prompt_settings_command(p_a_manager->p_settings);

    switch (command.type) 
    {
    case MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING:
    case MANAGER_SETTINGS_COMMAND_TYPE_DEFAULT_SETTINGS:
    case MANAGER_SETTINGS_COMMAND_TYPE_LOAD:
        response.has_output = FALSE;
        break; // not yet supported
    case MANAGER_SETTINGS_COMMAND_TYPE_START_GAME:
        p_a_manager->state = MANAGER_STATE_PRE_PLAY;
        response.has_output = FALSE;
        break;
    case MANAGER_SETTINGS_COMMAND_TYPE_QUIT:
        p_a_manager->state = MANAGER_STATE_QUIT;
        response.has_output = FALSE;
        break;
    }

    p_a_manager->settings_agent.handle_settigns_command_response(command, response); 
}

/**
 * handle the pre_play state. Initialize agents according to the
 * settings, and move to MANAGER_STATE_PLAY state. 
 * 
 * @param p_a_manager pointer to managed game
 */
void _MANAGER_handle_pre_play(MANAGER_managed_game_t* p_a_manager)
{
    /*TODO: implement this, for now we just start a two player game*/
    p_a_manager->play_agents[0] = p_a_manager->settings_agent;
    p_a_manager->play_agents[1] = p_a_manager->settings_agent;
    p_a_manager->state          = MANAGER_STATE_PLAY;
}

void _MANAGER_handle_play(MANAGER_managed_game_t* p_a_manager)
{
    MANAGER_agent_play_command_response_t response;
    MANAGER_agent_play_command_t command;

    COLOR game_current_player = GAME_current_player(p_a_manager->p_board);
    command = p_a_manager->play_agents[game_current_player].prompt_play_command(p_a_manager->p_board); 

    switch (command.type) 
    {
    case MANAGER_PLAY_COMMAND_TYPE_MOVE:
        GAME_MOVE_RESULTS_E move_result = GAME_make_move(p_a_manager->p_board, command.data.move);
        response.has_output = TRUE;
        response.output.move_result = move_result;
    }

    p_a_manager->play_agents[current_player].handle_play_command_response(command, response)
}

MANAGER_managed_game_t * MANAGER_new_managed_game(MANAGER_agent_t settings_agent)
{
    MANAGER_managed_game_t * p_manager = (MANAGER_managed_game_t *) malloc(sizeof(MANAGER_managed_game_t));
    assert(p_manager != NULL); // TODO: not assert here.

    p_manager->settings_agent = settings_agent; // keep the settings agent
    p_manager->state = MANAGER_STATE_INIT;      // and start at the init state
    p_manager->p_board = GAME_new_board();      // allocate a board
    assert (p_manager->p_board != NULL);        // TODO: not assert here.
    p_manager->p_settings = NULL;               // TBD

    return p_manager;
}

void MANAGER_free_managed_game(MANAGER_managed_game_t* p_a_manager)
{
    if (p_a_manager == NULL)
    {
        return;
    }

    GAME_free_board(p_a_manager->p_board) // free the board
    if (p_a_manager->p_settings != NULL) // if there are settings
    {
        free(p_a_manager->p_settings); // free them
    } // [TODO YVW] switch this with a dedicated function in SETTINGS.c

    free(p_a_manager);  // and only then free the game
}

void MANAGER_start_game(MANAGER_managed_game_t * p_a_manager)
{
    p_a_manager->state = MANAGER_STATE_SETTINGS;      // move to settings state

    while (p_a_manager->state != MANAGER_STATE_QUIT)  // main game loop
    {
        switch (p_a_manager->state)
        {
        case MANAGER_STATE_SETTINGS:
            _MANAGER_handle_settings(p_a_manager);
            break;
        case MANAGER_STATE_PRE_PLAY:
            _MANAGER_handle_pre_play(p_a_manager);
            break;
        case MANAGER_STATE_PLAY:
            _MANAGER_handle_play(p_a_manager);
            break;
        case MANAGER_STATE_QUIT:
            break;
        default:
            assert(0); // invalid state
        }
    }

    // State is now MANAGER_STATE_QUIT
    MANAGER_free_managed_game(p_a_manager);
}
