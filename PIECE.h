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
#define PIECE_MAX_OFFSETS     (9) // 1 for sentinel

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
    PIECE_TYPE_E    type;                           // piece type
    int             value;                          // Value of AI evaluation [YVW TODO: Maybe move to AI]
    char            letters[NUM_PLAYERS];           // letters per color      [YVW TODO: Maybe move to CLI]
    int             offsets[PIECE_MAX_OFFSETS];     // possible move offsets + sentinel value 0.
    BOOL            slides;                         // if TRUE, the piece can move it's offsets multiple times
    BOOL            can_promote_to;                 // True if the piece can be promoted to.
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
        .offsets        = { SQ_UP(SQ_UP(SQ_LEFT(0))),
                            SQ_UP(SQ_UP(SQ_RIGHT(0))),
                            SQ_LEFT(SQ_LEFT(SQ_UP(0))),
                            SQ_LEFT(SQ_LEFT(SQ_DOWN(0))),
                            SQ_DOWN(SQ_DOWN(SQ_LEFT(0))),
                            SQ_DOWN(SQ_DOWN(SQ_RIGHT(0))),
                            SQ_RIGHT(SQ_RIGHT(SQ_UP(0))),
                            SQ_RIGHT(SQ_RIGHT(SQ_DOWN(0))),
                            PIECE_OFFSET_SENTINEL 
                          },
        .slides         = FALSE,
        .can_promote_to = TRUE
    },
    {   // BISHOP
        .type           = PIECE_TYPE_BISHOP,
        .value          = 3,
        .letters        = { 'B', 'b'},
        .offsets        = { SQ_UP(SQ_LEFT(0)),
                            SQ_UP(SQ_RIGHT(0)),
                            SQ_DOWN(SQ_LEFT(0)),
                            SQ_DOWN(SQ_RIGHT(0)),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = TRUE,
        .can_promote_to = TRUE
    },
    {   // ROOK
        .type           = PIECE_TYPE_ROOK,
        .value          = 5,
        .letters        = { 'R', 'r'},
        .offsets        = { SQ_UP(0),
                            SQ_RIGHT(0),
                            SQ_LEFT(0),
                            SQ_DOWN(0),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = TRUE,
        .can_promote_to = TRUE
    },
    {   // QUEEN
        .type           = PIECE_TYPE_QUEEN,
        .value          = 9,
        .letters        = { 'Q', 'q'},
        .offsets        = { SQ_UP(0),
                            SQ_RIGHT(0),
                            SQ_LEFT(0),
                            SQ_DOWN(0),
                            SQ_UP(SQ_LEFT(0)),
                            SQ_UP(SQ_RIGHT(0)),
                            SQ_DOWN(SQ_LEFT(0)),
                            SQ_DOWN(SQ_RIGHT(0)),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = TRUE,
        .can_promote_to = TRUE
    },
    {   // KING
        .type           = PIECE_TYPE_KING,
        .value          = 100,
        .letters        = { 'K', 'k'},
        .offsets        = { SQ_UP(0),
                            SQ_RIGHT(0),
                            SQ_LEFT(0),
                            SQ_DOWN(0),
                            SQ_UP(SQ_LEFT(0)),
                            SQ_UP(SQ_RIGHT(0)),
                            SQ_DOWN(SQ_LEFT(0)),
                            SQ_DOWN(SQ_RIGHT(0)),
                            PIECE_OFFSET_SENTINEL
                          },
        .slides         = FALSE,
        .can_promote_to = FALSE
    },
};

#endif /*PIECE_IMP*/
