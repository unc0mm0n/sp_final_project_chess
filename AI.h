/**
 * AI.h
 *
 * The AI agent used by the game
 */

#ifndef AI_IMP
#define AI_IMP

#include "GAME.h"
#include "MANAGER.h"

#define AI_MAX_SCORE (10000)
#define AI_MIN_SCORE (-AI_MAX_SCORE)
/**
 * Enum of available AI difficulties
 */
typedef enum AI_DIFFICULTY_S
{
    AI_DIFFICULTY_NOOB = 1,
    AI_DIFFICULTY_EASY,
    AI_DIFFICULTY_MODERATE,
    AI_DIFFICULTY_HARD,
    AI_DIFFICULTY_EXPERT 
} AI_DIFFICULTY_E;

/**
 * Game move with score
 */
typedef struct AI_move_score_s
{
    int score;
    GAME_move_t move;
} AI_move_score_t;

/**
 * Return a play agent for the AI at given difficulty.
 *
 * @param a_difficulty AI agent difficulty.
 *
 * @return MANAGER_play_agent_t AI play agent.
 */
MANAGER_play_agent_t AI_get_play_agent(AI_DIFFICULTY_E a_difficulty);

/**
 * Search the board for a move to play and return it.
 *
 * @param p_a_board pointer to board.
 * @param depth depth to search for move.
 * @param heuristic a function accepting a board and returning a heuristic value for the board.
 *
 * @return GAME_move_t move chosen by the computer
 */
GAME_move_t AI_search_move(const GAME_board_t* p_a_board, int depth, int (*heuristic)(const GAME_board_t*));
#endif /*AI_IMP*/
