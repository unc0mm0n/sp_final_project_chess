/**
 * GAME.h 
 *  
 * The main part of the game. 
 */

#ifndef GAME_IMP
#define GAME_IMP

#include "PIECE.h"
#include "DEFS.h"

#define GAME_BOARD_ARR_SIZE   (128) // Size of board arrays.
#define GAME_HISTORY_SIZE     (400) // Up to 200 moves should almost always be enough..

/**
 * Allowed castle types for bitmask
 */
typedef struct GAME_CASTLE_BM_S
{
    GAME_CASTLE_W_KINGSIDE  = 1,
    GAME_CASTLE_W_QUEENSIDE = 2,
    GAME_CASTLE_B_KINGSIDE  = 4,
    GAME_CASTLE_B_QUEENSIDE = 8,

} GAME_CASTLE_BM_E;

/**
 * Allowed special behaviour types for bitmask
 */
typedef struct GAME_SPECIAL_BM_S
{
    GAME_CASTLE_W_KINGSIDE  = 1,
    GAME_CASTLE_W_QUEENSIDE = 2,
    GAME_CASTLE_B_KINGSIDE  = 4,
    GAME_CASTLE_B_QUEENSIDE = 8,

} GAME_SPECIAL_BM_E;

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
 * History move used to maintain more complete game history and 
 * undo moves. 
 */
typedef struct GAME_history_move_s
{
    GAME_move_t move;      // move that was played
    PIECE_TYPE_E capture;  // piece that was captured
    int special_bm;        // Bitmask indicating special behaviour (see GAME_SPECIAL_BM_E)
    int castle_bm;         // Bitmask indicating available castling (see GAME_CASTLE_BM_E)

} GAME_history_move_t;

/**
 * Game board object to hold a complete game of chess
 */
typedef struct GAME_board_s
{
    PIECE_type_e pieces[GAME_BOARD_ARR_SIZE];        // array of pieces in squares
    int colors[GAME_BOARD_ARR_SIZE];                 // array of colors of pieces in squares
    square ep;                                       // Square into which taking with en-passant is possible
    int castle_bm;                                   // Bitmask indicating available castling (see GAME_CASTLE_BM_E)
    GAME_history_move_t history[GAME_HISTORY_SIZE];  // Game history to undo moves
} GAME_board_t;

#endif /*GAME_IMP*/
