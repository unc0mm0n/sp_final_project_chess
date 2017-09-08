#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "AI.h"
#include "PIECE.h"
#include "GAME.h"
#include "MANAGER.h"
#include "CLI.h"
#include "HEAP.h"

static int gs_calcs;
static BOOL gs_print = FALSE; // if TRUE, will print moves.
static int gs_move_score; // used to keep default order if no score function is given.

/* sadly in C we don't have partial functions, so we have 5 different agents.
 * On the bright side this is a better way to do this if we would ever want to increase
 * variety between difficulties
 */

void _AI_free_heap_blocks(AI_heap_blocks_t* p_hb)
{
    HEAP_free_heap(p_hb->p_heap);
    for (size_t i=0; i < p_hb->total_move_blocks; i++)
    {
        free(p_hb->move_blocks[i]);
    }
    free(p_hb);
}

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

int _AI_expert_calculate_score_heuristic(const GAME_board_t* p_board, COLOR max_player)
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
    square op_king_sq;
    for (int rank = 0; rank < NUM_RANKS; rank++) // iterate over all squared, adding their score
    {
        int rank_score = (max_player == WHITE) ? rank: 7 - rank; // moving forward is good

        for (int file = 0; file < NUM_FILES; file++)
        {
            int file_score = 4 - abs(4 - file); // center files are good
            PIECE_desc_t desc = PIECE_desc_lut[p_board->pieces[SQ_FROM_FILE_RANK(file, rank)]];
            COLOR color = p_board->colors[SQ_FROM_FILE_RANK(file,rank)];
            if ( color == max_player)
            {
                assert(! (desc.type == PIECE_TYPE_EMPTY)); // if there is a color there must be a piece

                score += desc.value * 1000;
                if (desc.type == PIECE_TYPE_KING)
                {
                    score -= 40* ( file_score + rank_score); // kings should not move forward
                    //                    king_sq = SQ_FROM_FILE_RANK(file, rank);
                }
                else if (desc.type == PIECE_TYPE_PAWN)
                {
                    //                    score += 10 * (file_score + rank_score); // pawns should move forward
                }
                else
                {
                    //                  score += 10 * ( file_score + 2 * rank_score); // everything else should move to the center
                }
            }
            else if (color == OTHER_COLOR(max_player)) // reverse for opponent
            {
                score -= desc.value * 1000;
                op_king_sq = SQ_FROM_FILE_RANK(file,rank);
            }
            else
            {
                assert(desc.type == PIECE_TYPE_EMPTY); // if there is no color there must be no piece
            }
        }

    }

    for (int rank = 0; rank < NUM_RANKS; rank++) // we do a second iteration, trying to move close to the king
    {
        for (int file = 0; file < NUM_FILES; file++)
        {
            PIECE_desc_t desc = PIECE_desc_lut[p_board->pieces[SQ_FROM_FILE_RANK(file, rank)]];
            COLOR color = p_board->colors[SQ_FROM_FILE_RANK(file,rank)];
            if ( color == max_player)
            {
                if (desc.type != PIECE_TYPE_KING)
                {
                    score -= 20 * (abs(rank - SQ_TO_RANK(op_king_sq)) + abs(file - SQ_TO_FILE(op_king_sq)));
                }
            }
            else if (color == OTHER_COLOR(max_player)) // reverse for opponent
            {

                if (desc.type != PIECE_TYPE_KING)
                {
                    //                   score -= 10 * (abs(rank - SQ_TO_RANK(king_sq)) + abs(file - SQ_TO_FILE(king_sq)));
                }
            }
        }
    }
    return score;
}

int _AI_default_move_score(const GAME_move_analysis_t* analysis)
{
    assert(analysis->verdict == GAME_MOVE_VERDICT_LEGAL); // just so there are no complains of unused variables
    gs_move_score--;
    return gs_move_score;
}

int _AI_expert_move_score(const GAME_move_analysis_t* analysis)
{
    int base_score = 0;
    if (analysis->special_bm & GAME_SPECIAL_CASTLE)
    {
        base_score += 1000; // we want castles to be checked first.
    }
    if (analysis->special_bm & GAME_SPECIAL_CAPTURE)
    {
        // next we want to capture the best piece, with the worst piece.
        base_score += 100 * PIECE_desc_lut[analysis->capture].value - 10 * PIECE_desc_lut[analysis->piece].value;
    }
    if (analysis->special_bm & GAME_SPECIAL_CHECK)
    {
        base_score += 100; // next are checks (which might come before pawn captures)
    }
    if (analysis->special_bm & GAME_SPECIAL_UNDER_ATTACK)
    {
        base_score -= 1; // we don't want to be threatned, if possible.
    }

    // finally we tart from moves from the center to the center.
    base_score -= abs(4-SQ_TO_FILE(analysis->move.from)) + abs(4-SQ_TO_RANK(analysis->move.from));
    base_score -= 2 * (abs(4-SQ_TO_FILE(analysis->move.to)) + abs(4-SQ_TO_RANK(analysis->move.to)));
    return base_score;
}

/**
 * generate moves and put them in a heap, according to score.
 * each move should be freed individually before or after being popped!
 * This takes ~20k bytes of memory per search depth, which is not too bad.
 */
AI_heap_blocks_t* _AI_gen_moves(GAME_board_t* p_board, int (*score)(const GAME_move_analysis_t* analysis))
{
    AI_heap_blocks_t* p_hb = malloc(sizeof(AI_heap_blocks_t));

    p_hb->total_move_blocks = 0;

    HEAP_t* p_heap;
    GAME_move_analysis_t* p_moves;

    p_heap = HEAP_create_heap(AI_MOVE_HEAP_SIZE);
    assert (p_heap != NULL);

    for (int rank = 0; rank < NUM_RANKS; rank++) // iterate over every square
    {
        for (int file = 0; file < NUM_FILES; file++)
        {
            p_moves = GAME_gen_moves_from_sq(p_board, SQ_FROM_FILE_RANK(file,rank)); // and generate moves of pieces on it
            if (p_moves == NULL) // if no friendly piece on square, move to next
            {
                continue;
            }
            p_hb->move_blocks[p_hb->total_move_blocks] = p_moves;
            p_hb->total_move_blocks++;

            while (p_moves->verdict == GAME_MOVE_VERDICT_LEGAL)
            {
                int m_score = score(p_moves);
                HEAP_push(p_heap, p_moves, m_score);
                p_moves++;
            }
        }
    }
    p_hb->p_heap = p_heap;
    return p_hb;
}

/**
 * Normal minimax with alpha-beta pruning using the negamax implementation.
 * heuristic function gives a board's value, score function gives a move's value for priority.
 * Both of these are customized in expert difficulty
 */
AI_move_score_t _AI_minimax(GAME_board_t* p_board, int depth, int a, int b, int (*heuristic)(const GAME_board_t* p_board, COLOR max_player),
        int (*score)(const GAME_move_analysis_t* analysis))
{
    GAME_move_analysis_t* p_move;
    AI_heap_blocks_t * p_hb;
    HEAP_t* p_heap;

    AI_move_score_t v;     // result to be returned
    AI_move_score_t tmp_v; // result of recursive call

    if (depth == 0 || GAME_get_result(p_board) != GAME_RESULT_PLAYING) // base case - terminal position or ran out of depth
    {
        gs_calcs++;
        v.score = heuristic(p_board, GAME_current_player(p_board));
        return v;
    }
    v.score = AI_MIN_SCORE;
    p_hb = _AI_gen_moves(p_board, score);
    p_heap = p_hb->p_heap;
    while (p_heap->size > 0)
    {
        p_move = (GAME_move_analysis_t*) HEAP_pop(p_heap);
        assert(GAME_make_move(p_board, p_move->move).played); // play the move, moves from gen move should be legal;
        tmp_v =  _AI_minimax(p_board, depth - 1, -b, -a, heuristic, score);  // search for the opponent, and take the worst move for him
        GAME_undo_move(p_board);                                     // and finally undo the move
        if (v.score < -tmp_v.score)                                  // now if move is better keep it and it's score
        {
            v.score = -tmp_v.score;
            v.move = *p_move;
        }

                a = MAX(a, v.score);                                          // and don't forget to update alpha value

        if (b <= a)                                                   // and check if we can prune
        {
            break;
        }
    }
    _AI_free_heap_blocks(p_hb);
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

    gs_move_score = 100000;
    gs_calcs = 0;

    if (a_difficulty != AI_DIFFICULTY_EXPERT)
    {
        move_score = _AI_minimax(p_board_copy, a_difficulty, AI_MIN_SCORE, AI_MAX_SCORE, _AI_calculate_score_heuristic, _AI_default_move_score);
    }
    else
    {
        move_score = _AI_minimax(p_board_copy, 5, AI_MIN_SCORE, AI_MAX_SCORE, _AI_expert_calculate_score_heuristic, _AI_expert_move_score);
    }

    if ((move_score.move.special_bm & GAME_SPECIAL_CASTLE) > 0)
    {
        command.type = MANAGER_PLAY_COMMAND_TYPE_CASTLE;
        if (SQ_TO_FILE(move_score.move.move.to) == SQ_TO_FILE(C1)) // queenside castle
        {
            command.data.sq = SQ_LEFT(SQ_LEFT(move_score.move.move.to));
        }
        else
        {
            command.data.sq = SQ_RIGHT(move_score.move.move.to);
        }
    }
    else
    {
        command.data.move = move_score.move.move; // take the move with the best score
    }
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
    return _AI_prompt_play_command(p_board, AI_DIFFICULTY_EXPERT);
}

void _AI_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response)
{
    GAME_RESULT_E game_result;
    GAME_move_analysis_t analysis;
    if (command.type == MANAGER_PLAY_COMMAND_TYPE_MOVE)
    {
        analysis = response.output.move_data.move_result.move_analysis;
        game_result = response.output.move_data.game_result;
        assert(response.has_output && analysis.verdict == GAME_MOVE_VERDICT_LEGAL);
    }
    else if (command.type == MANAGER_PLAY_COMMAND_TYPE_CASTLE)
    {
        analysis = response.output.castle_data.move.move_analysis;
        game_result = response.output.castle_data.game_result;
        assert(response.has_output && analysis.verdict == GAME_MOVE_VERDICT_LEGAL);
    }

    if (gs_print) // make all computer specific prints to screen.
    {
        char from_str[6], to_str[6];

        square from = analysis.move.from;
        square to = analysis.move.to;

        if ((analysis.special_bm & GAME_SPECIAL_CASTLE) == 0) // no castle 
        {
            CLI_sq_to_str(from, from_str);
            CLI_sq_to_str(to, to_str);

            char* name = PIECE_desc_lut[analysis.piece].name;
            printf("Computer: move %s at %s to %s", name, from_str, to_str);
            if ((analysis.special_bm & GAME_SPECIAL_PROMOTE) > 0)
            {
                printf(" and promote to %s", PIECE_desc_lut[analysis.move.promote].name);
            }
            printf("\n");
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
        switch(game_result)
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
