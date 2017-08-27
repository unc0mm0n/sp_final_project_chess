#include <time.h>
#include <assert.h>
#include <stdlib.h>

#include "AI.h"
#include "PIECE.h"
#include "GAME.h"
#include "MANAGER.h"
#include "CLI.h"

/* sadly in C we don't have partial functions, so we have 5 different agents.
 * On the bright side this is a better way to do this if we would ever want to increase
 * variety between difficulties
 */

MANAGER_agent_play_command_t _AI_prompt_play_command(const GAME_board_t* p_a_board, AI_DIFFICULTY_E a_difficulty)
{
    assert(a_difficulty != AI_DIFFICULTY_EXPERT);
    assert(p_a_board->ep != 0);

    MANAGER_agent_play_command_t command;
    GAME_board_t * p_board_copy = GAME_copy_board(p_a_board);
    GAME_move_full_t * p_moves;
    GAME_move_full_t * tmp;
    GAME_move_t move;

    command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;

    if (GAME_get_result(p_a_board) != GAME_RESULT_PLAYING)
    {
        command.type = MANAGER_PLAY_COMMAND_TYPE_QUIT;
        free(p_board_copy);
        return command;
    }

    CLI_print_board(p_a_board);
    
    while (TRUE)
    {
    for (int file = 0; file < NUM_FILES; file++)
    {
        for (int rank = 0; rank < NUM_RANKS; rank++)
        {
            p_moves = GAME_gen_moves_from_sq(p_board_copy, SQ_FROM_FILE_RANK(file, rank));
            if (p_moves != NULL)
            {
                tmp = p_moves;
                while(tmp->valid == TRUE)
                {
                    if (rand() < RAND_MAX / 10)
                    {
                        move.to = tmp->move.to;
                        move.from = tmp->move.from;
                        move.promote = PIECE_TYPE_QUEEN;
                        command.data.move = move;
                        free(p_moves);
                        GAME_free_board(p_board_copy);
                        return command;
                    }
                    tmp++;
                }
                free(p_moves);
            }
        }
    }
    }

    //GAME_free_board(p_board_copy);
    //return command;
}

MANAGER_agent_play_command_t _AI_prompt_play_command_noob(const GAME_board_t* p_board)
{
    return _AI_prompt_play_command(p_board, AI_DIFFICULTY_NOOB);
}


MANAGER_agent_play_command_t _AI_prompt_play_command_easy(const GAME_board_t* p_board)
{
    return _AI_prompt_play_command(p_board, AI_DIFFICULTY_EASY);
}

MANAGER_agent_play_command_t _AI_prompt_play_command_moderate(const GAME_board_t* p_board)
{
    return _AI_prompt_play_command(p_board, AI_DIFFICULTY_MODERATE);
}


MANAGER_agent_play_command_t _AI_prompt_play_command_hard(const GAME_board_t* p_board)
{
    return _AI_prompt_play_command(p_board, AI_DIFFICULTY_HARD);
}


MANAGER_agent_play_command_t _AI_prompt_play_command_expert(const GAME_board_t* p_board)
{
    return _AI_prompt_play_command(p_board, AI_DIFFICULTY_EXPERT);
}

void _AI_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response)
{
    assert(command.type == MANAGER_PLAY_COMMAND_TYPE_MOVE || command.type == MANAGER_PLAY_COMMAND_TYPE_QUIT);
    assert(!response.has_output || response.output.move_result == GAME_MOVE_RESULT_TYPE_SUCCESS);
    return;
}

MANAGER_play_agent_t AI_get_play_agent(AI_DIFFICULTY_E a_difficulty)
{
    srand(time(NULL));
    MANAGER_play_agent_t agent;
    agent.handle_play_command_response = _AI_handle_play_command_response;

    switch(a_difficulty)
    {
    case AI_DIFFICULTY_NOOB:
        agent.prompt_play_command = _AI_prompt_play_command_noob;
        break;
    case AI_DIFFICULTY_EASY:
        agent.prompt_play_command = _AI_prompt_play_command_easy;
        break;
    case AI_DIFFICULTY_MODERATE:
        agent.prompt_play_command = _AI_prompt_play_command_moderate;
        break;
    case AI_DIFFICULTY_HARD:
        agent.prompt_play_command = _AI_prompt_play_command_hard;
        break;
    case AI_DIFFICULTY_EXPERT:
        agent.prompt_play_command = _AI_prompt_play_command_expert;
        break;
    }
    
    return agent;
}
