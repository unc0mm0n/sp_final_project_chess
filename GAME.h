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
#define GAME_HISTORY_SIZE       (200)  // counted in half-moves, which is 1 player move.
                                       // enough to hold the entire game. The player limitations  on undos
                                       // should be enforced by the manager (and do not apply to the AI during
                                       // calculations).
#define GAME_MAX_POSSIBLE_MOVES (65)   // Maximum possible moves of a theoretical piece + sentinel

#define GAME_NO_EP              ((square) 0x88) // value which cannot pass SQUARE_IS_LEGAL for no ep
#define GAME_NO_SQUARE          ((square) 0x88)

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
    GAME_SPECIAL_CAPTURE          = 1,
    GAME_SPECIAL_EP_CAPTURE       = 2,
    GAME_SPECIAL_PROMOTE          = 4,
    GAME_SPECIAL_CASTLE           = 8,
    GAME_SPECIAL_CHECK            = 16,
    GAME_SPECIAL_UNDER_ATTACK     = 32,
    GAME_SPECIAL_PAWN_DOUBLE_MOVE = 64
} GAME_SPECIAL_BM_E;

/**
 * Possible results of attempting to make a move.
 */
typedef enum GAME_MOVE_VERDICT_S
{
    GAME_MOVE_VERDICT_NONE,
    GAME_MOVE_VERDICT_LEGAL,
    GAME_MOVE_VERDICT_ILLEGAL_SQUARE,   // original square is illegal.
    GAME_MOVE_VERDICT_NO_PIECE,          // there is no piece (of the current player's color) in the original square
    GAME_MOVE_VERDICT_ILLEGAL_MOVE,      // the move with the piece is illegal
    GAME_MOVE_VERDICT_KING_THREATENED,   // Move leaves king in check
    GAME_MOVE_VERDICT_ILLEGAL_PROMOTION, // the move is a promotion into an illegal piece
    GAME_MOVE_VERDICT_ILLEGAL_CASTLE     // the move is an illegal castle

} GAME_MOVE_VERDICT_E;

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
typedef struct GAME_move_analysis_s
{
    GAME_MOVE_VERDICT_E verdict;    // is move a valid piece move (not necesserily legal)
    GAME_move_t  move;               // move that was played
    COLOR        color;
    PIECE_TYPE_E piece;             // the piece that moved
    PIECE_TYPE_E capture;           // piece that was captured
    int special_bm;                 // Bitmask indicating special behaviour (see GAME_SPECIAL_BM_E)

} GAME_move_analysis_t;

/**
 * Returned whenever make-move is called with the result of the 
 * move. 
 */
typedef struct GAME_move_result_s
{
    BOOL played;                        // True if the move was played on the board.
    GAME_move_analysis_t move_analysis;     // the move analysis, so that the exact issues can be parsed.

} GAME_move_result_t;

/**
 * Struct to remember important history parameters (which move 
 * was played, and what were the ep/castle status). 
 */
typedef struct GAME_history_s
{
    GAME_move_analysis_t move;                 // The move that was played with all of it's parameters
    int              castle_bm[2];         // Bitmask indicating available castling (see GAME_CASTLE_BM_E)
    square           ep;                   // whether ep was legal
} GAME_history_t;

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
    GAME_history_t history[GAME_HISTORY_SIZE];     // Game history to undo moves
} GAME_board_t;

/**
 * Generate a new board.
 *  
 * @return GAME_board_t* board for game at move 0 with white to 
 *         play.
 */
GAME_board_t * GAME_new_board();

/**
 * free the board and all resources allocated to it (NOT 
 * INCLUDING returned possible_move arrays). 
 * Is NULL safe, and will return on null input. 
 *  
 * @param p_a_board pointer to board 
 */
void GAME_free_board(GAME_board_t * p_a_board);

/**
 * Generate a copy of given board.
 *
 * @param p_a_board pointer to board.  
 * @return GAME_board_t* copy of board 
 */
GAME_board_t* GAME_copy_board(const GAME_board_t* p_a_board);

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
BOOL GAME_is_checked(const GAME_board_t* p_a_board, COLOR color);

/**
 * Make a single move in the game if legal.
 * 
 * @param p_a_board pointer to board
 * @param a_move move to make
 * 
 * @return GAME_MOVE_VERDICT_E result of trying to make the 
 *         move
 */
GAME_move_result_t GAME_make_move(GAME_board_t *p_a_board, GAME_move_t a_move); 

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
GAME_move_analysis_t GAME_undo_move(GAME_board_t * p_a_board);

/**
 * Check the result of the game and return it.
 * The board itself is otherwise unaware of the result, as it does not
 * affect gameplay.
 * Therefore, this should be called by the object managing the game
 * probably as part of it's main loop, probably after every move.
 *
 * @param p_a_board pointer to board
 * 
 * @return GAME_RESULT_E result of the game as defined in GAME_RESULT_E
 */
GAME_RESULT_E GAME_get_result(const GAME_board_t * p_a_board);

/**
 * Return an array of size GAME_MAX_POSSIBLE_MOVES of possible 
 * moves for piece at a_from square. Allocates memory that must be
 * freed!
 *
 * End of output will be the first element for which move_analysis.valid
 * is FALSE.
 *  
 * Return  NULL if there is no piece of the current player to play's
 * color on the square. 
 * 
 * @param p_a_board pointer to board
 * @param a_from square from which the piece moves
 * 
 * @return GAME_move_analysis_t* Array of possible moves or NULL
 */
GAME_move_analysis_t * GAME_gen_moves_from_sq(GAME_board_t * p_a_board, square a_from);

/** 
 * Return the color of the current player to play. 
 *  
 * @param p_a_board pointer to board 
 *  
 * @return COLOR the color of the player to play 
 */
COLOR GAME_current_player(const GAME_board_t * p_a_board);

/** 
 * Return the letter of the piece at 
 *  
 * @param p_a_board pointer to board 
 * @param a_sq square to get letter at
 *  
 * @return char the letter of the piece on the square
 */
char GAME_piece_letter_at(const GAME_board_t * p_a_board, square a_sq);

#endif /*GAME_IMP*/
