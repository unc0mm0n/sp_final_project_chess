/** 
 * PIECE.h 
 *  
 * Definitions regarding a single piece, with no direct relation 
 * to the board. 
 */

#ifndef PIECE_IMP
#define PIECE_IMP

#include "DEFS.h"

#define PIECE_OFFSET_SENTINEL (0)

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
    int             value;          // Value of AI evaluation [YVW TODO: Maybe move from here]
    char            letters[2];     // letters per color
    int             offsets[9];     // possible move offsets + sentinel value 0.
    BOOL            slides;         // if TRUE, the piece can move it's offsets multiple times
    BOOL            can_promote_to; // True if the piece can be promoted to.
} PIECE_desc_t;

/** 
 * LUT of descriptors for all pieces by PIECE_TYPE_E for quick lookup
 * While we do not need to specify the sentinel value for each piece,
 * it is more elegant, especially if some day someone would want a piece that can remain in place.
 */
static const PIECE_desc_t PIECE_desc_lut[PIECE_TYPE_MAX] = 
{
    {   // EMTPY SPACE
        .type           = PIECE_TYPE_EMPTY,
        .value          = -1,
        .letters        = { '_', '_'},
        .offsets        = {PIECE_OFFSET_SENTINEL},
        .slides         = FALSE,
        .can_promote_to = FALSE
    },
    {   // PAWN
        .type           = PIECE_TYPE_PAWN,
        .value          = 1,
        .letters        = { 'M', 'm'},
        .offsets        = {PIECE_OFFSET_SENTINEL},              // pawns are handled individually
        .slides         = FALSE,
        .can_promote_to = FALSE
    },
    {   // KNIGHT
        .type           = PIECE_TYPE_KNIGHT,
        .value          = 3,
        .letters        = { 'N', 'n'},
        .offsets        = { SQUARE_UP(SQUARE_UP(SQUARE_LEFT(0))),
                            SQUARE_UP(SQUARE_UP(SQUARE_RIGHT(0))),
                            SQUARE_LEFT(SQUARE_LEFT(SQUARE_UP(0))),
                            SQUARE_LEFT(SQUARE_LEFT(SQUARE_DOWN(0))),
                            SQUARE_DOWN(SQUARE_DOWN(SQUARE_LEFT(0))),
                            SQUARE_DOWN(SQUARE_DOWN(SQUARE_RIGHT(0))),
                            SQUARE_RIGHT(SQUARE_RIGHT(SQUARE_UP(0))),
                            SQUARE_RIGHT(SQUARE_RIGHT(SQUARE_DOWN(0))),
                            PIECE_OFFSET_SENTINEL 
                          },
        .slides         = FALSE,
        .can_promote_to = TRUE
    },
    {   // BISHOP
        .type           = PIECE_TYPE_BISHOP,
        .value          = 3,
        .letters        = { 'B', 'b'},
        .offsets        = { SQUARE_UP(SQUARE_LEFT(0)),
                            SQUARE_UP(SQUARE_RIGHT(0)),
                            SQUARE_DOWN(SQUARE_LEFT(0)),
                            SQUARE_DOWN(SQUARE_RIGHT(0)),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = TRUE,
        .can_promote_to = TRUE
    },
    {   // ROOK
        .type           = PIECE_TYPE_ROOK,
        .value          = 5,
        .letters        = { 'R', 'r'},
        .offsets        = { SQUARE_UP(0),
                            SQUARE_RIGHT(0),
                            SQUARE_LEFT(0),
                            SQUARE_DOWN(0),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = TRUE,
        .can_promote_to = TRUE
    },
    {   // QUEEN
        .type           = PIECE_TYPE_EMPTY,
        .value          = 9,
        .letters        = { 'Q', 'q'},
        .offsets        = { SQUARE_UP(0),
                            SQUARE_RIGHT(0),
                            SQUARE_LEFT(0),
                            SQUARE_DOWN(0),
                            SQUARE_UP(SQUARE_LEFT(0)),
                            SQUARE_UP(SQUARE_RIGHT(0)),
                            SQUARE_DOWN(SQUARE_LEFT(0)),
                            SQUARE_DOWN(SQUARE_RIGHT(0)),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = TRUE,
        .can_promote_to = TRUE
    },
    {   // KING
        .type           = PIECE_TYPE_EMPTY,
        .value          = 400,
        .letters        = { 'K', 'k'},
        .offsets        = { SQUARE_UP(0),
                            SQUARE_RIGHT(0),
                            SQUARE_LEFT(0),
                            SQUARE_DOWN(0),
                            SQUARE_UP(SQUARE_LEFT(0)),
                            SQUARE_UP(SQUARE_RIGHT(0)),
                            SQUARE_DOWN(SQUARE_LEFT(0)),
                            SQUARE_DOWN(SQUARE_RIGHT(0)),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = FALSE,
        .can_promote_to = FALSE
    },
};

#endif /*PIECE_IMP*/
