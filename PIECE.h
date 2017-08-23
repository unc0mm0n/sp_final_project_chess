/** 
 * PIECE.h 
 *  
 * Definitions regarding a single piece, with no direct relation 
 * to the board. 
 */

#ifndef PIECE_IMP
#define PIECE_IMP

/** 
 * Possible piece types. 
 */
typedef enum PIECE_TYPE_S
{
    PIECE_TYPE_EMPTY,
    PIECE_TYPE_PAWN,
    PIECE_TYPE_KNIGHT,
    PIECE_TYPE_BISHOP,
    PIECE_TYPE_ROOK,
    PIECE_TYPE_QUEEN,
    PIECE_TYPE_KING,
    PIECE_TYPE_MAX         // Holds total size for LUT
} PIECE_TYPE_E;

/**
 * Piece descriptor for all relevant information regarding the 
 * piece. 
 * 
 */
typedef struct PIECE_desc_s
{
    PIECE_TYPE_E    type;           // piece type
    int             value;          // Value of AI evaluation
    char            letters[2];     // letters per color
    int             offsets[9];     // possible move offsets + sentinel value 0.
    BOOL            can_promote_to; // True if the piece can be promoted to.
} PIECE_desc_t;

/**
 * Get the above descriptor for given piece type. 
 * @param piece piece type
 * 
 * @return PIECE_desc_t* pointer to piece descriptor.
 */
PIECE_desc_t* PIECE_get_descriptor(PIECE_TYPE_E piece);

#endif /*PIECE_IMP*/
