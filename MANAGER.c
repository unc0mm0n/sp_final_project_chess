#include <stdlib.h>
#include <assert.h>
#include <stdio.h> // tmp

#include "MANAGER.h"
#include "AI.h"

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
    p_a_manager->play_agents[WHITE] = p_a_manager->settings_agent.get_play_agent();
    p_a_manager->play_agents[BLACK] = AI_get_play_agent(1);
    p_a_manager->state          = MANAGER_STATE_PLAY;
}

void _MANAGER_handle_play(MANAGER_managed_game_t* p_a_manager)
{
    MANAGER_agent_play_command_response_t response;
    MANAGER_agent_play_command_t command;
    GAME_move_result_t move_result;
    
    response.has_output = FALSE; // we assume no output
    COLOR game_current_player = GAME_current_player(p_a_manager->p_board); // the player at the start of the command

    // prompt for action from the current player.
    command = p_a_manager->play_agents[game_current_player].prompt_play_command(p_a_manager->p_board); 
    
    switch (command.type) 
    {
    case MANAGER_PLAY_COMMAND_TYPE_MOVE: // attempt to make move and notify results   
        /* DEBUG PRINTS*/
        printf("square: from %x, to %x, promote %d\n", command.data.move.from, command.data.move.to, command.data.move.promote);
        if (p_a_manager->p_board->turn > 1)
        {
            GAME_move_analysis_t lm = p_a_manager->p_board->history[p_a_manager->p_board->turn-1].move;
            printf("special_bm: %x castle_bm_w: %x castle_bm_b: %x ep: %x turn:%d \n", lm.special_bm, p_a_manager->p_board->castle_bm[WHITE], p_a_manager->p_board->castle_bm[BLACK], p_a_manager->p_board->ep, p_a_manager->p_board->turn);
        }
        printf("current player %d\n", GAME_current_player(p_a_manager->p_board));
        /* END DEBUG PRINTS*/

        move_result = GAME_make_move(p_a_manager->p_board, command.data.move);

        response.output.move_result = move_result; 
        response.has_output = TRUE;
        break;
    case MANAGER_PLAY_COMMAND_TYPE_QUIT: // change to quit state
        printf("Quitting...\n");
        p_a_manager->state = MANAGER_STATE_QUIT;
        break;
    default:
        assert(0);
        break;
    }

    p_a_manager->play_agents[game_current_player].handle_play_command_response(command, response);
}

MANAGER_managed_game_t * MANAGER_new_managed_game(MANAGER_settings_agent_t settings_agent)
{
    MANAGER_managed_game_t * p_manager = (MANAGER_managed_game_t *) malloc(sizeof(MANAGER_managed_game_t));
    assert(p_manager != NULL); // TODO: not assert here.

    p_manager->settings_agent = settings_agent;         // keep the settings agent
    p_manager->state = MANAGER_STATE_INIT;              // and start at the init state
    p_manager->p_board = GAME_new_board();              // allocate a board
    assert (p_manager->p_board != NULL);                // TODO: possibly not assert here.
    p_manager->p_settings = SETTINGS_new_settings();    // TBD
    assert (p_manager->p_settings != NULL);             // TODO: possibly not assert here.

    return p_manager;
}

void MANAGER_free_managed_game(MANAGER_managed_game_t* p_a_manager)
{
    if (p_a_manager == NULL)
    {
        return;
    }

    GAME_free_board(p_a_manager->p_board); // free the board
    SETTINGS_free_settings(p_a_manager->p_settings);

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
