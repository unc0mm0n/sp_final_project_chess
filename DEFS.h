/** 
 * DEFS.h 
 *  
 * Basic definitions used throughout the entire project.
 */

#ifndef DEFS_IMP
#define DEFS_IMP

#define BOOL int
#define TRUE (1)
#define FALSE (0)

#define BLACK          (0)           // used to indicate the black player.
#define WHITE          (1)           // used to indicate the white player.
#define NUM_PLAYERS    (2)           // number of players in a game of chess.
#define OTHER_COLOR(c) ((c) ^ 1)     // flip between BLACK and WHITE

#define RANK_NUM (8)    // number of ranks in a game of chess
#define FILE_NUM (8)    // number of files in a game of chess

typedef uint8_t square; // bits 0-3 bits indicate the file and bits 4-6 indicate the rank. Bits 3 and 7 are for control.
typedef uint32_t color; // should be BLACK or WHITE

#define SQUARE_FROM_FILE_RANK(f, r) ((square) (((r) << 4) + (f)))  // get a square using file and rank.
#define FILE_FROM_SQUARE(sq) ((int) ((sq) & 7))                    // get a file using square
#define RANK_FROM_SQUARE(sq) ((int) (((sq) >> 4) & 7))             // get a rank using square
#define SQUARE_IS_LEGAL(sq)  (!((sq) & 0x88))                      // check if square is a legal square (using control bits, assuming type square)

#define SQUARE_UP(sq)    ((sq) + (1<<4))    // increase the rank by 1, giving the square above.
#define SQUARE_DOWN(sq)  ((sq) - (1<<4))    // reduce the rank by 1, giving the square below.
#define SQUARE_RIGHT(sq) ((sq) + 1)         // increas the file by 1, giving the square to the right.
#define SQUARE_LEFT(sq)  ((sq) - 1)         // reduce the rank by 1, giving the square to the left.

#endif /*DEFS_IMP*/
