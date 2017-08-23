/**
 * GAME.h 
 *  
 * The main part of the game. 
 */

#ifndef GAME_IMP
#define GAME_IMP

#include "PIECE.h"
#include "DEFS.h"

#define GAME_BOARD_ARR_SIZE     (128)  // Size of board arrays.
#define GAME_HISTORY_SIZE       (400)  // counted in half-moves, which is 1 player move.
                                       // enough to hold the entire game. The player limitations  on undos
                                       // should be enforced by the manager (and do not apply to the AI during
                                       // calculations).
#define GAME_MAX_POSSIBLE_MOVES (65)   // Maximum possible moves of a theoretical piece + sentinel

#define GAME_NO_EP              ((square) 0x88) // value which cannot pass SQUARE_IS_LEGAL for no ep

// Disable castle c in bitmask bm
#define DISABLE_CASTLE(bm, c)   ((bm) &(~ (c)))

/**
 * Allowed castle types for bitmask
 */
typedef enum GAME_CASTLE_BM_S
{
    GAME_CASTLE_NONE      = 0,
    GAME_CASTLE_KINGSIDE  = 1,
    GAME_CASTLE_QUEENSIDE = 2,
    GAME_CASTLE_ALL       = 3  // Used to initialize the game.
} GAME_CASTLE_BM_E;


/**
 * Allowed special behaviour types for bitmask
 */
typedef enum GAME_SPECIAL_BM_S
{
    GAME_SPECIAL_CAPTURE        = 1,
    GAME_SPECIAL_EP_CAPTURE     = 2,
    GAME_SPECIAL_PROMOTE        = 4,
    GAME_SPECIAL_CASTLE         = 8,
    GAME_SPECIAL_CHECK          = 16,
    GAME_SPECIAL_UNDER_ATTACK   = 32
} GAME_SPECIAL_BM_E;

/**
 * Possible results of attempting to make a move.
 */
typedef enum GAME_MOVE_RESULTS_S
{
    GAME_MOVE_RESULT_SUCCESS,
    GAME_MOVE_RESULT_ILLEGALE_SQUARE,   // original square is illegal.
    GAME_MOVE_RESULT_NO_PIECE,          // there is no piece (of the current player's color) in the original square
    GAME_MOVE_RESULT_ILLEGAL_MOVE       // the move with the piece is illegal

} GAME_MOVE_RESULTS_E;

/**
 * Possible game results.
 */
typedef enum GAME_RESULT_S
{
    GAME_RESULT_PLAYING,    // The game is not over
    GAME_RESULT_BLACK_WINS, // The black player won
    GAME_RESULT_WHITE_WINS, // The white player won
    GAME_RESULT_DRAW        // The game was drawn (only stalemate is supported) 
} GAME_RESULT_E;
/**
 * Game move used to tell the board to move pieces.
 */
typedef struct GAME_move_s
{
    square from;          // Square to move from
    square to;            // Square to move to
    PIECE_TYPE_E promote; // Piece to promote to, not looked at if not promoting a pawn
} GAME_move_t;

/**
 * Move used to maintain more complete game history, undo moves 
 * and whenever information about moves should be returned from 
 * a function call. 
 */
typedef struct GAME_move_full_s
{
    GAME_move_t move;         // move that was played
    PIECE_TYPE_E capture;     // piece that was captured
    int special_bm;           // Bitmask indicating special behaviour (see GAME_SPECIAL_BM_E)
    int castle_bm[2];         // Bitmask indicating available castling (see GAME_CASTLE_BM_E)

} GAME_move_full_t;

/**
 * Game board object to hold a complete game of chess.
 *
 * Size: 9k, 8k of which is hisotry with default value 400.
 */
typedef struct GAME_board_s
{
    PIECE_TYPE_E pieces[GAME_BOARD_ARR_SIZE];        // array of pieces in squares
    COLOR colors[GAME_BOARD_ARR_SIZE];               // array of colors of pieces in squares
    square ep;                                       // Square into which taking with en-passant is possible
    int castle_bm[NUM_PLAYERS];                      // Bitmask indicating available castling per player (see GAME_CASTLE_BM_E)
    int turn;                                        // Current turn of the game, starts at 1.
    GAME_RESULT_E result;                            // result of the game.
    GAME_move_full_t history[GAME_HISTORY_SIZE];     // Game history to undo moves
} GAME_board_t;

/**
 * Generate a new board.
 *  
 * @return GAME_board_t* board for game at move 0 with white to 
 *         play.
 */
GAME_board_t * GAME_new_board();

/**
 * Free the board and all resources allocated to it (NOT 
 * INCLUDING returned possible_move arrays). 
 *  
 * @param p_a_board pointer to board 
 *  
 * @return TRUE on success, FALSE on fail 
 */
BOOL GAME_free_board(GAME_board_t * p_a_board);


/**
 * Return TRUE if the player at given color attacks given 
 * square. 
 * 
 * @param color color of player to test 
 * @param sq square to test for 
 * @param p_a_board pointer to board to test on 
 * 
 * @return BOOL true if square is attacked.
 */
BOOL GAME_is_attacking(const GAME_board_t* p_a_board, COLOR color, square sq);

/**
 * Return TRUE if the player at given color is in check.
 * 
 * @param color color of player to test 
 * @param p_a_board pointer to board to test on
 * 
 * @return BOOL true if player is in check
 */
BOOL GAME_player_is_in_check(const GAME_board_t* p_a_board, COLOR color);

/**
 * Make a single move in the game if legal.
 * 
 * @param p_a_board pointer to board
 * @param a_move move to make
 * 
 * @return GAME_MOVE_RESULTS_E result of trying to make the move
 */
GAME_MOVE_RESULTS_E GAME_make_move(GAME_board_t *p_a_board, GAME_move_t a_move); 

/**
 * Undo a single move, and return the history_move struct of the 
 * move undone. 
 * 
 * Return NULL if no move was undone. 
 *  
 * @param p_a_board pointer to board
 * 
 * @return GAME_history_move_t history_move of move undone or 
 *         NULL.
 */
GAME_move_full_t GAME_undo_move(GAME_board_t * p_a_board);

/**
 * Return an array of size GAME_MAX_POSSIBLE_MOVES of possible 
 * movees for piece at a_from square. With sentinel NULL values 
 * at the end of output. 
 *  
 * Return  NULL if there is no piece of the current player's 
 * color on the square. 
 * 
 * @param p_a_board pointer to board
 * @param a_from square from which the piece moves
 * 
 * @return GAME_move_full_t* Array of possible moves or NULL
 */
GAME_move_full_t * GAME_get_all_moves(const GAME_board_t * p_a_board, square a_from);

/** 
 * Return the color of the current player to play. 
 *  
 * @param p_a_board pointer to board 
 *  
 * @return COLOR the color of the player to play 
 */
inline COLOR GAME_current_player(const GAME_board_t * p_a_board);

/** 
 * Return the letter of the piece at 
 *  
 * @param p_a_board pointer to board 
 * @param a_sq square to get letter at
 *  
 * @return char the letter of the piece on the square
 */
inline char GAME_piece_letter_at(const GAME_board_t * p_a_board, square a_sq);

#endif /*GAME_IMP*/
