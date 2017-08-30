/** 
 * DEFS.h 
 *  
 * Basic definitions used throughout the entire project.
 */

#ifndef DEFS_IMP
#define DEFS_IMP

#include <stdint.h>

/* General defines */

#define BOOL int
#define TRUE (1)
#define FALSE (0)

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* Basic chess defines */

#define COLOR          int
#define NO_COLOR       (4)           // used to indicate that it's neither white nur black. 
                                     // (4 is used to be far enough with no shared bits)
#define BLACK          (0)           // used to indicate the black player.
#define WHITE          (1)           // used to indicate the white player.
#define NUM_PLAYERS    (2)           // number of players in a game of chess.

#define IS_COLOR(c)    ((c) == 1 || (c) == 0)       // check if character is color.
#define OTHER_COLOR(c) (IS_COLOR(c) ? (c) ^ 1 : c)  // flip between BLACK and WHITE, NO_COLOR remains the same

#define NUM_RANKS (8)    // number of ranks in a game of chess
#define NUM_FILES (8)    // number of files in a game of chess

/* Defines to work with squares */

typedef uint8_t square; // bits 0-3 bits indicate the file and bits 4-6 indicate the rank. Bits 3 and 7 are for control.

#define SQ_FROM_FILE_RANK(f, r) ((square) (((7 - (r)) << 4) + (f)))  // get a square using file and rank.
#define SQ_TO_FILE(sq) ((int) ((sq) & 7))                    // get a file using square
#define SQ_TO_RANK(sq) (7 - (int) (((sq) >> 4) & 7))             // get a rank using square
#define SQ_IS_LEGAL(sq)  (!((sq) & 0x88))                      // check if square is a legal square (using control bits, assuming type square)

#define SQ_UP(sq)           ((sq) - (1<<4))         // reduce the rank by 1, giving the square above.
#define SQ_DOWN(sq)         ((sq) + (1<<4))         // increase the rank by 1, giving the square below.
#define SQ_RIGHT(sq)        ((sq) + 1)              // increase the file by 1, giving the square to the right.
#define SQ_LEFT(sq)         ((sq) - 1)              // reduce the rank by 1, giving the square to the left.
#define SQ_UP_LEFT(sq)      SQ_UP(SQ_LEFT(sq))      // move up then left
#define SQ_UP_RIGHT(sq)     SQ_UP(SQ_RIGHT(sq))     // move up then right
#define SQ_DOWN_LEFT(sq)    SQ_DOWN(SQ_LEFT(sq))    // move down then left
#define SQ_DOWN_RIGHT(sq)   SQ_DOWN(SQ_RIGHT(sq))   // move down then right

#define SQ_IS_DIAG_UP(f, t)   ((t) == SQ_UP_LEFT(f) || (t) == SQ_UP_RIGHT(f))       // if t is diagonal up from f
#define SQ_IS_DIAG_DOWN(f, t) ((t) == SQ_DOWN_LEFT(f) || (t) == SQ_DOWN_RIGHT(f))   // if t is diagonal down from f

/* Defines to test squares */
#define SQ_IS_RANK(sq, r) (SQ_TO_RANK(sq) == (r))

#define WHITE_LAST_RANK (NUM_RANKS - 1)                             // Rank in which white promotes
#define IS_WHITE_LAST_RANK(sq) SQ_IS_RANK(sq, WHITE_LAST_RANK)      // Test that is the rank
#define BLACK_LAST_RANK (0)                                         // Rank in which black promots
#define IS_BLACK_LAST_RANK(sq) SQ_IS_RANK(sq, BLACK_LAST_RANK)      // Test that is the rank
#define LAST_RANK(c) ((c)? WHITE_LAST_RANK:BLACK_LAST_RANK)         // Return the correct last rank for color

#define WHITE_PAWN_RANK        (1)                                  // Rank in which white pawns start
#define IS_WHITE_PAWN_RANK(sq) SQ_IS_RANK(sq, WHITE_PAWN_RANK)      // Test that is the rank                                             
#define BLACK_PAWN_RANK        (NUM_RANKS - 2)                      // Rank in which black pawns start
#define IS_BLACK_PAWN_RANK(sq) SQ_IS_RANK(sq, BLACK_PAWN_RANK)      // Test that is the rank
#define IS_PAWN_RANK(c, sq)    ((c) == WHITE ? IS_WHITE_PAWN_RANK(sq) : IS_BLACK_PAWN_RANK(sq))

/* Important squares (piece rows, mostly for castling tests) */                                          
#define A1 SQ_FROM_FILE_RANK(0,0)
#define B1 SQ_FROM_FILE_RANK(1,0)
#define C1 SQ_FROM_FILE_RANK(2,0)
#define D1 SQ_FROM_FILE_RANK(3,0)
#define E1 SQ_FROM_FILE_RANK(4,0)
#define F1 SQ_FROM_FILE_RANK(5,0)
#define G1 SQ_FROM_FILE_RANK(6,0)
#define H1 SQ_FROM_FILE_RANK(7,0)

#define A8 SQ_FROM_FILE_RANK(0,7)
#define B8 SQ_FROM_FILE_RANK(1,7)
#define C8 SQ_FROM_FILE_RANK(2,7)
#define D8 SQ_FROM_FILE_RANK(3,7)
#define E8 SQ_FROM_FILE_RANK(4,7)
#define F8 SQ_FROM_FILE_RANK(5,7)
#define G8 SQ_FROM_FILE_RANK(6,7)
#define H8 SQ_FROM_FILE_RANK(7,7)

#define KING_START(c) ((c) == WHITE ? E1: E8)


#endif /*DEFS_IMP*/


