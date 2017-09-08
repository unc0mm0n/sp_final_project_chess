#include <stdlib.h>
#include <assert.h>
#include <stdio.h> // tmp

#include "MANAGER.h"
#include "AI.h"
#include "SDL_INTERFACE.h"

/**
 * Handle a game loop iteration in the MANAGER_STATE_SETTINGS 
 * state.
 * 
 * @param p_a_manager pointer to managed game
 */
void _MANAGER_handle_settings(MANAGER_managed_game_t *p_a_manager)
{
    MANAGER_agent_settings_command_t command;
    MANAGER_agent_settings_command_response_t response;

    command = p_a_manager->settings_agent.prompt_settings_command(p_a_manager->p_settings);

    switch (command.type)
    {
    case MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING:
        {
            SETTINGS_CHANGE_RESULT_E result;
            result = SETTINGS_change_setting(p_a_manager->p_settings, command.data.change_setting.setting, command.data.change_setting.value);
            response.has_output = TRUE;
            response.output.settings_change_result = result;
            break;
        }
    case MANAGER_SETTINGS_COMMAND_TYPE_DEFAULT_SETTINGS:
        {
            SETTINGS_reset_settings(p_a_manager->p_settings);
            response.has_output = FALSE;
            break;
        }
    case MANAGER_SETTINGS_COMMAND_TYPE_LOAD:
    case MANAGER_SETTINGS_COMMAND_TYPE_NONE:
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
void _MANAGER_handle_pre_play(MANAGER_managed_game_t *p_a_manager)
{
    /*TODO: implement this, for now we just start a two player game*/

    if (p_a_manager->p_settings->game_mode == 2)
    {
        p_a_manager->play_agents[WHITE] = p_a_manager->settings_agent.get_play_agent();
        p_a_manager->play_agents[BLACK] = p_a_manager->settings_agent.get_play_agent();
    }
    else if (p_a_manager->p_settings->game_mode == 1)
    {
        p_a_manager->play_agents[p_a_manager->p_settings->user_color] = p_a_manager->settings_agent.get_play_agent();
        p_a_manager->play_agents[OTHER_COLOR(p_a_manager->p_settings->user_color)] = AI_get_play_agent(p_a_manager->p_settings->difficulty);
        //p_a_manager->play_agents[BLACK] = AI_get_play_agent(2);
    }
    else // temporary hidden game mode
    {
        p_a_manager->play_agents[WHITE] = AI_get_play_agent(AI_DIFFICULTY_EXPERT);
        p_a_manager->play_agents[BLACK] = AI_get_play_agent(p_a_manager->p_settings->difficulty);
    }
    p_a_manager->state          = MANAGER_STATE_PLAY;
}

void _MANAGER_handle_play(MANAGER_managed_game_t *p_a_manager)
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

        move_result = GAME_make_move(p_a_manager->p_board, command.data.move);
        if ((move_result.played) && (move_result.move_analysis.special_bm & GAME_SPECIAL_CASTLE ) > 0)
        { // castle is only allowed in a weird way..
            GAME_undo_move(p_a_manager->p_board);
            move_result.played = FALSE;
            move_result.move_analysis.verdict = GAME_MOVE_VERDICT_ILLEGAL_MOVE;
        }
        response.output.move_data.move_result = move_result;
        response.output.move_data.game_result = GAME_get_result(p_a_manager->p_board);
        response.has_output = TRUE;
        p_a_manager->undo_count = MIN(MANAGER_UNDO_COUNT, p_a_manager->undo_count + 1);
        if (GAME_get_result(p_a_manager->p_board) != GAME_RESULT_PLAYING)
        {
            p_a_manager->state = MANAGER_STATE_QUIT;
        }
        break;
    case MANAGER_PLAY_COMMAND_TYPE_QUIT: // change to quit state
        p_a_manager->state = MANAGER_STATE_QUIT;
        break;
    case MANAGER_PLAY_COMMAND_TYPE_UNDO:
        if (p_a_manager->p_settings->game_mode != 1)
        {
            response.output.undo_data.undo_result = MANAGER_UNDO_RESULT_FAIL_TWO_PLAYERS;
        } else if (p_a_manager->p_board->turn <= 2 || p_a_manager->undo_count < 2)
        {
            response.output.undo_data.undo_result = MANAGER_UNDO_RESULT_FAIL_NO_HISTORY;
        } else
        {
            response.output.undo_data.undone_moves[0] = GAME_undo_move(p_a_manager->p_board);
            response.output.undo_data.undone_moves[1] = GAME_undo_move(p_a_manager->p_board);
            response.output.undo_data.undo_result = MANAGER_UNDO_RESULT_SUCCESS;
            p_a_manager->undo_count -= 2;
        }
        response.has_output = TRUE;
        break;
    case MANAGER_PLAY_COMMAND_TYPE_NONE:
        response.has_output = FALSE;
        break;
    case MANAGER_PLAY_COMMAND_TYPE_RESET:
        GAME_free_board(p_a_manager->p_board);
        p_a_manager->p_board = GAME_new_board();
        p_a_manager->state = MANAGER_STATE_SETTINGS;
        p_a_manager->undo_count = 0;
        break;
    case MANAGER_PLAY_COMMAND_TYPE_GET_MOVES:
        response.has_output = TRUE;
        response.output.get_moves_data.display_hints = p_a_manager->p_settings->difficulty <= AI_DIFFICULTY_EASY;
        response.output.get_moves_data.player_color = game_current_player;
        response.output.get_moves_data.moves = GAME_gen_moves_from_sq(p_a_manager->p_board, command.data.sq);
        break;
    case MANAGER_PLAY_COMMAND_TYPE_CASTLE:
        {
        GAME_move_t move;
        GAME_move_result_t result;
        square sq = command.data.sq;

        response.has_output = TRUE;

        move.from = 0;
        move.to = 0;

        if (p_a_manager->p_board->pieces[command.data.sq] != PIECE_TYPE_ROOK // rook should be in castle target
            || p_a_manager->p_board->colors[command.data.sq] != game_current_player)
        {
            response.output.castle_data.castle_result = MANAGER_CASTLE_RESULT_FAIL_NO_ROOK;
        }
        else if (SQ_TO_FILE(sq) == SQ_TO_FILE(A1)) // queenside castle
        {
            move.from = SQ_RIGHT(SQ_RIGHT(SQ_RIGHT(SQ_RIGHT(sq))));
            move.to = SQ_RIGHT(SQ_RIGHT(sq));
        }
        else // kingside castle
        {
            move.from = SQ_LEFT(SQ_LEFT(SQ_LEFT(sq)));
            move.to = SQ_LEFT(sq);
        }
        result = GAME_make_move(p_a_manager->p_board, move);

        if (!result.played)
        {
            response.output.castle_data.castle_result = MANAGER_CASTLE_RESULT_FAIL;
        }
        else if ((result.move_analysis.special_bm & GAME_SPECIAL_CASTLE) == 0) // a move was played, but it wasn't a castle. 
        {
            GAME_undo_move(p_a_manager->p_board);
            response.output.castle_data.castle_result = MANAGER_CASTLE_RESULT_FAIL;
        }
        else // castle was successful
        {
            response.output.castle_data.castle_result = MANAGER_CASTLE_RESULT_SUCCESS;
            response.output.castle_data.move = result;
            response.output.castle_data.game_result = GAME_get_result(p_a_manager->p_board);
            p_a_manager->undo_count = MIN(MANAGER_UNDO_COUNT, p_a_manager->undo_count + 1);
            if (GAME_get_result(p_a_manager->p_board) != GAME_RESULT_PLAYING)
            {
                p_a_manager->state = MANAGER_STATE_QUIT;
            }
        }
        break;
        }
    case MANAGER_PLAY_COMMAND_TYPE_RESTART:
        {
        GAME_free_board(p_a_manager->p_board);
        p_a_manager->p_board = GAME_new_board();
        p_a_manager->undo_count = 0;
        break;
        }
    default:
        assert(0);
        break;
    }

    p_a_manager->play_agents[game_current_player].handle_play_command_response(command, response);
}

MANAGER_managed_game_t* MANAGER_new_managed_game(MANAGER_settings_agent_t settings_agent, void (*quit)())
{
    MANAGER_managed_game_t *p_manager = (MANAGER_managed_game_t *)malloc(sizeof(MANAGER_managed_game_t));
    assert(p_manager != NULL); // TODO: not assert here.

    p_manager->settings_agent = settings_agent;         // keep the settings agent
    p_manager->state = MANAGER_STATE_INIT;              // and start at the init state
    p_manager->p_board = GAME_new_board();              // allocate a board
    assert(p_manager->p_board != NULL);                // TODO: possibly not assert here.
    p_manager->p_settings = SETTINGS_new_settings();    // TBD
    assert(p_manager->p_settings != NULL);             // TODO: possibly not assert here.
    p_manager->undo_count = 0;
    p_manager->handle_quit = quit;

    return p_manager;
}

void MANAGER_free_managed_game(MANAGER_managed_game_t *p_a_manager)
{
    if (p_a_manager == NULL)
    {
        return;
    }

    GAME_free_board(p_a_manager->p_board); // free the board
    SETTINGS_free_settings(p_a_manager->p_settings);

    free(p_a_manager);  // and only then free the game
}

void MANAGER_start_game(MANAGER_managed_game_t *p_a_manager)
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
    
    p_a_manager->handle_quit();
    // State is now MANAGER_STATE_QUIT
    MANAGER_free_managed_game(p_a_manager);
}
