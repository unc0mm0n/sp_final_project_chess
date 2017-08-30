#include <time.h> // temp
#include <assert.h>
#include <stdlib.h>
#include <stdio.h> // temp

#include "AI.h"
#include "PIECE.h"
#include "GAME.h"
#include "MANAGER.h"
#include "CLI.h"

/* sadly in C we don't have partial functions, so we have 5 different agents.
 * On the bright side this is a better way to do this if we would ever want to increase
 * variety between difficulties
 */

int _AI_calculate_score_heuristic(const GAME_board_t* p_board)
{

}

AI_move_score_t _AI_minimax(GAME_board_t* p_board, int depth, int a, int b, COLOR current_player)
{
    AI_move_score_t move_score;
    GAME_move_t move;
    int count;
    GAME_move_analysis_t* p_moves;

    AI_move_score_t v;     // result to be returned
    AI_move_score_t tmp_v; // result of recursive call

    if (depth == 0 || GAME_get_result(p_board) != GAME_RESULT_PLAYING)
    {
        move_score.score = _AI_calculate_score_heuristic(p_board);
        return move_score;
    }

    if (current_player == GAME_get_current_player(p_board)))
    {
        v.score = AI_MIN_SCORE;
        for (int file = 0; file < NUM_FILES; file++)
        {
            for (int row = 0; row < NUM_ROWS; row++)
            {
                p_moves = GAME_gen_moves_from_sq(p_board, SQ_FROM_FILE_RANK(file,rank));
                if (p_moves == NULL)
                {
                        free(p_moves);
                        continue;
                }

                count = 0;
                while (p_moves[count] != GAME_MOVE_VERDICT_ILLEGAL)
                {
                    GAME_make_move(p_board, p_moves[count].move);
                    tmp_v =  _AI_minimax(p_board, depth – 1, a, b, current_player));

                    if (v.score < tmp_v.score)
                    {
                        v.score = tmp_v.score;
                        v.move = tmp_v.move;
                    }

                    a = MAX(a, v.score);

                    if (b <= a) 
                    {
                            break; /* cut-off */
                    }
                    count++;
                }
                free(p_moves);
            }
        }
        return v;
    }
    else
    {
         v.score = AI_MAX_SCORE;
        for (int file = 0; file < NUM_FILES; file++)
        {
            for (int row = 0; row < NUM_ROWS; row++)
            {
                p_moves = GAME_gen_moves_from_sq(p_board, SQ_FROM_FILE_RANK(file,rank));
                if (p_moves == NULL)
                {
                        free(p_moves);
                        continue;
                }

                count = 0;
                while (p_moves[count] != GAME_MOVE_VERDICT_ILLEGAL)
                {
                    GAME_make_move(p_board, p_moves[count].move);
                    tmp_v =  _AI_minimax(p_board, depth – 1, a, b, current_player));

                    if (v.score > tmp_v.score)
                    {
                        v.score = tmp_v.score;
                        v.move = tmp_v.move;
                    }

                    b = MIN(b, v.score);

                    if (b <= a) 
                    {
                            break; /* cut-off */
                    }
                    count++;
                }
                free(p_moves);
            }
        }

        return v;
    }
}

MANAGER_agent_play_command_t _AI_prompt_play_command(const GAME_board_t* p_a_board, AI_DIFFICULTY_E a_difficulty)
{
    assert(a_difficulty != AI_DIFFICULTY_EXPERT);
    assert(p_a_board->ep != 0);

    MANAGER_agent_play_command_t command;
    GAME_board_t * p_board_copy = GAME_copy_board(p_a_board);
    GAME_move_analysis_t * p_moves;
    GAME_move_analysis_t * tmp;
    GAME_move_t move;

    command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;

    if (GAME_get_result(p_a_board) != GAME_RESULT_PLAYING)
    {
        printf("GAME RESULT: %d\n", GAME_get_result(p_a_board));
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
                    while(tmp->verdict == GAME_MOVE_VERDICT_LEGAL)
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
    assert(!response.has_output || response.output.move_result.move_analysis.verdict == GAME_MOVE_VERDICT_LEGAL); // computer must make legal moves
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