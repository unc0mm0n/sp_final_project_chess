#include <time.h> // temp
#include <assert.h>
#include <stdlib.h>
#include <stdio.h> // temp

#include "AI.h"
#include "PIECE.h"
#include "GAME.h"
#include "MANAGER.h"
#include "CLI.h"


static BOOL gs_print = TRUE; // if TRUE, will print moves.

/* sadly in C we don't have partial functions, so we have 5 different agents.
 * On the bright side this is a better way to do this if we would ever want to increase
 * variety between difficulties
 */

int _AI_calculate_score_heuristic(const GAME_board_t* p_board, COLOR max_player)
{
    GAME_RESULT_E result = GAME_get_result(p_board);

    switch (result)
    {
        case GAME_RESULT_PLAYING:
            break;
        case GAME_RESULT_WHITE_WINS:
            return (max_player == WHITE ? AI_MAX_SCORE - 1 : AI_MIN_SCORE + 1);
        case GAME_RESULT_BLACK_WINS:
            return (max_player == BLACK ? AI_MAX_SCORE - 1 : AI_MIN_SCORE + 1);
        case GAME_RESULT_DRAW:
            return 0;
    }

    int score = 0;
    int counter = 0;
    for (int file = 0; file < NUM_FILES; file++)
    {
        for (int rank = 0; rank < NUM_RANKS; rank++)
        {
            PIECE_desc_t desc = PIECE_desc_lut[p_board->pieces[SQ_FROM_FILE_RANK(file, rank)]];
            COLOR color = p_board->colors[SQ_FROM_FILE_RANK(file,rank)];
            //            printf("File %d, Rank %d, type %d, color: %d\n", file, rank, desc.type, color);
            //            printf("board piece: %d\n", p_board->pieces[SQ_FROM_FILE_RANK(file,rank)]);
            if ( color == max_player)
            {
                assert(! (desc.type == PIECE_TYPE_EMPTY));
                score += desc.value;
            }
            else if (color == OTHER_COLOR(max_player))
            {
                score -= desc.value;
                assert(! (desc.type == PIECE_TYPE_EMPTY));
            }
            else
            {
                counter++;
                assert(desc.type == PIECE_TYPE_EMPTY);
            }
        }

    }
    //    printf("\n================== score for ==================\n");
    //    CLI_print_board(p_board);
    //    printf("==================== %d =======================\n\n", score);
    return score;
}

AI_move_score_t _AI_minimax(GAME_board_t* p_board, int depth, int a, int b)
{
    //printf("==================================================\n");
    //CLI_print_board(p_board);
    int count;
    BOOL pruned = FALSE;
    GAME_move_analysis_t* p_moves;

    AI_move_score_t v;     // result to be returned
    AI_move_score_t tmp_v; // result of recursive call

    if (depth == 0 || GAME_get_result(p_board) != GAME_RESULT_PLAYING)
    {
        v.score = _AI_calculate_score_heuristic(p_board, GAME_current_player(p_board));
        return v;
    }
    v.score = AI_MIN_SCORE;
    for (int file = 0; file < NUM_FILES; file++)
    {
        for (int row = 0; row < NUM_RANKS; row++)
        {
            p_moves = GAME_gen_moves_from_sq(p_board, SQ_FROM_FILE_RANK(file,row));
            if (p_moves == NULL)
            {
                continue;
            }

            count = 0;
            while (p_moves[count].verdict == GAME_MOVE_VERDICT_LEGAL)
            {
                assert(GAME_make_move(p_board, p_moves[count].move).played); // moves from gen move should be legal;
                tmp_v =  _AI_minimax(p_board, depth - 1, -b, -a);
                GAME_undo_move(p_board);
                if (v.score < -tmp_v.score)
                {
                    v.score = -tmp_v.score;
                    v.move = p_moves[count].move;
                }

                a = MAX(a, v.score);

                if (b <= a) 
                {
                    pruned = TRUE;
                    break; /* cut-off */
                }
                count++;
            }
            free(p_moves);
            if (pruned)
            {
                break;
            }
        }
        if (pruned)
        {
            break;
        }
    }
    return v;
}

MANAGER_agent_play_command_t _AI_prompt_play_command(const GAME_board_t* p_a_board, AI_DIFFICULTY_E a_difficulty)
{
    assert(a_difficulty >= AI_DIFFICULTY_EASY);
    assert(p_a_board->ep >= 0);

    MANAGER_agent_play_command_t command;
    GAME_board_t * p_board_copy = GAME_copy_board(p_a_board);
    AI_move_score_t move_score;

    command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;

    move_score = _AI_minimax(p_board_copy, a_difficulty, AI_MIN_SCORE, AI_MAX_SCORE);
    command.data.move = move_score.move;
    GAME_free_board(p_board_copy);
    return command;
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
    return _AI_prompt_play_command(p_board, 6);
}

void _AI_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response)
{
    assert(command.type == MANAGER_PLAY_COMMAND_TYPE_MOVE);
    assert(response.output.move_data.move_result.played); // computer must make legal moves
    if (gs_print)
    {
        char from_str[6], to_str[6];
        square from = response.output.move_data.move_result.move_analysis.move.from;
        square to = response.output.move_data.move_result.move_analysis.move.to;

        CLI_sq_to_str(from, from_str);
        CLI_sq_to_str(to, to_str);

        char* name = PIECE_desc_lut[response.output.move_data.move_result.move_analysis.piece].name;
        printf("Computer: move %s at %s to %s\n", name, from_str, to_str);
        switch( response.output.move_data.game_result)
        {
            case GAME_RESULT_DRAW:
                printf("The game ends in a tie\n");
                break;
            case GAME_RESULT_BLACK_WINS:
                printf("Checkmate! black player wins the game\n");
                break;
            case GAME_RESULT_WHITE_WINS:
                printf("Checkmate! white player wins the game\n");
                break;
            default:
                if ((response.output.move_data.move_result.move_analysis.special_bm & GAME_SPECIAL_CHECK) > 0)
                {
                    printf("Check!\n");
                }
                break;
        }
    }
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
