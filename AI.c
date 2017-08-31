#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

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

    switch (result) // check if the board is over, in which case take an extreme result.
    {
        case GAME_RESULT_PLAYING:
            break;
        case GAME_RESULT_WHITE_WINS:
            return (max_player == WHITE ? AI_MAX_SCORE - 1 : AI_MIN_SCORE + 1); // 1 from the edge so that a move will still register
        case GAME_RESULT_BLACK_WINS:
            return (max_player == BLACK ? AI_MAX_SCORE - 1 : AI_MIN_SCORE + 1);
        case GAME_RESULT_DRAW:
            return 0; // possible - give draws negative score, to make games more interesting. Especially relevant if 50 moves draw is added.
    }

    int score = 0;
    int counter = 0;
    for (int file = 0; file < NUM_FILES; file++) // iterate over all squared, adding their score
    {
        for (int rank = 0; rank < NUM_RANKS; rank++)
        {
            PIECE_desc_t desc = PIECE_desc_lut[p_board->pieces[SQ_FROM_FILE_RANK(file, rank)]];
            COLOR color = p_board->colors[SQ_FROM_FILE_RANK(file,rank)];
            if ( color == max_player)
            {
                assert(! (desc.type == PIECE_TYPE_EMPTY)); // if there is a color there must be a piece
                score += desc.value;
            }
            else if (color == OTHER_COLOR(max_player))
            {
                score -= desc.value;
                assert(! (desc.type == PIECE_TYPE_EMPTY)); // if there is a color there must be a piece
            }
            else
            {
                counter++;
                assert(desc.type == PIECE_TYPE_EMPTY); // if there is no color there must be no piece
            }
        }

    }
    return score;
}

AI_move_score_t _AI_minimax(GAME_board_t* p_board, int depth, int a, int b, int (*heuristic)(const GAME_board_t* p_board, COLOR max_player))
{
    int count;
    BOOL pruned = FALSE;
    GAME_move_analysis_t* p_moves;

    AI_move_score_t v;     // result to be returned
    AI_move_score_t tmp_v; // result of recursive call

    if (depth == 0 || GAME_get_result(p_board) != GAME_RESULT_PLAYING) // base case - terminal position or ran out of depth
    {
        v.score = heuristic(p_board, GAME_current_player(p_board));
        return v;
    }
    v.score = AI_MIN_SCORE;
    for (int rank = 0; rank < NUM_RANKS; file++) // iterate over every square
    {
        for (int file = 0; file < NUM_FILES; rank++)
        {
            p_moves = GAME_gen_moves_from_sq(p_board, SQ_FROM_FILE_RANK(file,rank)); // and generate moves of pieces on it
            if (p_moves == NULL) // if no friendly piece on square, move to next
            {
                continue;
            }

            count = 0;
            while (p_moves[count].verdict == GAME_MOVE_VERDICT_LEGAL)
            {
                assert(GAME_make_move(p_board, p_moves[count].move).played); // play the move, moves from gen move should be legal;
                tmp_v =  _AI_minimax(p_board, depth - 1, -b, -a);            // search for the opponent, and take the worst move for him
                GAME_undo_move(p_board);                                     // and finally undo the move
                if (v.score < -tmp_v.score)                                  // now if move is better keep it and it's score
                {
                    v.score = -tmp_v.score;
                    v.move = p_moves[count].move;
                }

                a = MAX(a, v.score);                                          // and don't forget to update alpha value

                if (b <= a)                                                   // and check if we can prune
                {
                    pruned = TRUE;
                    break;
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

    MANAGER_agent_play_command_t command;
    GAME_board_t * p_board_copy = GAME_copy_board(p_a_board); // do the testing on a copy of the board
                                                              // (we can't modify the board directly, as it's const)
                                                              // (this also prevents "cheating")
    AI_move_score_t move_score;

    command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE; // AI only plays moves

    // todo: add different heuristic for stronger AI, if not different minmax altogether.
    move_score = _AI_minimax(p_board_copy, a_difficulty, AI_MIN_SCORE, AI_MAX_SCORE, _AI_calculate_score_heuristic);
    command.data.move = move_score.move; // take the move with the best score
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
    if (gs_print) // make all computer specific prints to screen.
    {
        GAME_move_analysis_t analysis = response.output.move_data.move_result.move_analysis;
        char from_str[6], to_str[6];

        square from = move_analysis.move.from;
        square to = move_analysis.move.to;

        if (analysis.special_bm & GAME_SPECIAL_CASTLE == 0) // no castle 
        {
            CLI_sq_to_str(from, from_str);
            CLI_sq_to_str(to, to_str);

            char* name = PIECE_desc_lut[response.output.move_data.move_result.move_analysis.piece].name;
            printf("Computer: move %s at %s to %s\n", name, from_str, to_str);
        }
        else
        {
            if (SQ_TO_FILE(to) == SQ_TO_FILE(C1)) // queenside castle
            {
                to = SQ_LEFT(SQ_LEFT(to));
            }
            else // kingside castle
            {
                to = SQ_RIGHT(to);
            }

            CLI_sq_to_str(from, from_str);
            CLI_sq_to_str(to, to_str);

            printf("Computer: castle King at %s and Rook at %s\n", from_str, to_str);
        }
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

void AI_set_print(BOOL value)
{
    gs_print = value;
}
