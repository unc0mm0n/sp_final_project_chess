#include <stdlib.h>

#include "GAME.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h> // tmp

/**
 * private function to initialize a new board
 */
void _GAME_init_pieces(GAME_board_t* p_board)
{
    PIECE_TYPE_E E = PIECE_TYPE_EMPTY;
    PIECE_TYPE_E P = PIECE_TYPE_PAWN;
    PIECE_TYPE_E N = PIECE_TYPE_KNIGHT;
    PIECE_TYPE_E B = PIECE_TYPE_BISHOP;
    PIECE_TYPE_E R = PIECE_TYPE_ROOK;
    PIECE_TYPE_E Q = PIECE_TYPE_QUEEN;
    PIECE_TYPE_E K = PIECE_TYPE_KING;

    PIECE_TYPE_E init_pieces[GAME_BOARD_ARR_SIZE] =
    {
        R,N,B,Q,K,B,N,R, E,E,E,E,E,E,E,E,
        P,P,P,P,P,P,P,P, E,E,E,E,E,E,E,E,
        E,E,E,E,E,E,E,E, E,E,E,E,E,E,E,E,
        E,E,E,E,E,E,E,E, E,E,E,E,E,E,E,E,
        E,E,E,E,E,E,E,E, E,E,E,E,E,E,E,E,
        E,E,E,E,E,E,E,E, E,E,E,E,E,E,E,E,
        P,P,P,P,P,P,P,P, E,E,E,E,E,E,E,E, 
        R,N,B,Q,K,B,N,R, E,E,E,E,E,E,E,E, 
    };

    memcpy(p_board->pieces, init_pieces, sizeof(p_board->pieces));
}

void _GAME_init_color(GAME_board_t* p_board)
{
    int B = BLACK;
    int W = WHITE;
    int N = NO_COLOR;
    int init_colors[GAME_BOARD_ARR_SIZE] =
    {
        B,B,B,B,B,B B B, N,N,N,N,N,N,N,N,
        B,B,B,B,B,B B B, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N N N, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N N N, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N N N, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N N N, N,N,N,N,N,N,N,N,
        W,W,W,W,W,W W W, N,N,N,N,N,N,N,N,
        W,W,W,W,W,W W W, N,N,N,N,N,N,N,N
    };
    
    memcpy (p_board->colors, init_colors, sizeof(p_board->colors));
}

#define PRUNE(cond) if(!(cond)){return NULL;}   // helper local define to prune moves.

/**
 * Return true if the piece can theoretically move there on an 
 * empty board. 
 * As always does not handle the case of pawns. 
 *  
 * Here is the main point for which we we need the LUT defined 
 * in PIECES. 
 */
BOOL _GAME_is_allowed_piece_movement(GAME_board_t* p_a_board, GAME_move_t a_move)
{
    PIECE_desc_t piece_desc = PIECE_desc_lut[PIECE_TYPE];
    for (int i = 0; i < PIECE_MAX_OFFSET; i++)
    {
        if (piece_desc.offsets[i] == PIECE_OFFSET_SENTINEL)
        {
            return FALSE; // We have checked all piece offsets and haven't found a match.
        }

        square tmp = a_move.to; // currently looked at square.
        offset = piece_desc.offsets[i]; // offset to move the piece at each step.

        while (SQ_IS_LEGAL(tmp))) 
        {
            tmp += offset;
            if (tmp == a_move.from)
            {
                return TRUE;        // We have found a move sequence leading to the destination.
            }
            if (!piece_desc.slides) // Can only move once in this offset, switch to next offset.
            {
                break;
            }      
        }

    }
}

/**
 * Private function to analayze if move corresponds to 
 * actual playable move in the game, this checks if the move is 
 * pseudo-legal (can be made according to movement rules, but 
 * doesn't verify the attained position is legal. e.g. king is 
 * not checked, including before during and after castle) and 
 * the events of capture/castle/ep/promote and returns a 
 * move_full accordingly or NULL if the move is not legal. 
 *  
 * We already assume here that the move is from a valid square 
 * with a correctly colored piece to a valid square. 
 */
GAME_move_full_t _GAME_analayze_move(const GAME_board_t * p_a_board, GAME_move_t a_move)
{
    COLOR player = GAME_current_player(p_a_board);

    GAME_move_full_t full_move;
    PIECE_desc_t piece_desc;  //piece descriptor

    full_move.special_bm = 0;
    memcpy(full_move.castle_bm, p_a_board->castle_bm, sizeof(full_move.castle_bm));
    full_move.move = a_move;
    full_move.ep   = GAME_NO_EP

    BOOL is_captrue = (p_a_board->colors[a_move.to] == OTHER_COLOR(player));

    if (p_a_board->pieces[a_move.from] == PIECE_TYPE_PAWN) // Pawns get special treatment
    {
        BOOL is_move;
        BOOL is_diag_move;
        BOOL is_ep = (a_move.to == p_a_board->ep); // Is en passant capture
        int  new_ep; // Calculate new ep value. Different if black or white moved.
        switch (player)
        {
        case WHITE:
            is_move = (a_move.to == SQ_UP(a_move.from))                                           // move up
            is_double_move = (IS_WHITE_PAWN_RANK(sq) && a_move.to == SQ_UP(SQ_UP(a_move.from)));  // or move twice up from initial square
            new_ep = SQ_UP(a_move.from);
            is_diag_move = SQ_IS_DIAG_UP(a_move.from, a_move.to);                                 // or move diagonally if capture or ep
            break;
        case BLACK:
            is_move = (a_move.to == SQ_DOWN(a_move.from))                                            // move down
            is_double_move = (IS_BLACK_PAWN_RANK(sq) && a_move.to == SQ_DOWN(SQ_DOWN(a_move.from))); // or move twice down from initial square
            new_ep = SQ_DOWN(a_move.from);
            is_diag_move = SQ_IS_DIAG_DOWN(a_move.from, a_move.to);                                  // or move diagonally if capture or ep
            break;
        case default:
            assert(0); // invalid color
        }

        PRUNE(is_move || is_double_move || (is_diag_movement && (is_capture || is_ep)));

        if (SQ_TO_RANK(a_move.to) == LAST_RANK(player)) // promotion
        {
            PRUNE(PIECE_desc_lut[a_move.promote].can_promote_to);

            full_move.special_bm |= GAME_SPECIAL_PROMOTE;
        }

        if (is_ep) // en passant capture
        {
            full_move.special_bm |= GAME_SPECIAL_EP_CAPTURE;
        }

        if (is_double_move) // to update ep is possible.
        {
            full_move.ep = new_ep;
        }
    }
    else if ((p_a_board->pieces[a_move.from] == PIECE_TYPE_KING) && (a_move.to == SQ_LEFT(SQ_LEFT(a_move.from))))   // queenside castle
    {
        PRUNE(p_a_board->castle_bm[player] & GAME_CASTLE_QUEENSIDE)         // make sure castle is legal
        PRUNE(p_a_board->pieces[SQ_LEFT(a_move.to)] == PIECE_TYPE_EMTPY)    // make sure B1/B8 is empty
        PRUNE(p_a_board->pieces[a_move.to] == PIECE_TYPE_EMTPY)             // make sure C1/C8 is empty (castles can't capture)
        PRUNE(p_a_board->pieces[SQ_RIGHT(a_move.to)] == PIECE_TYPE_EMTPY)   // make sure D1/D8 is empty
        assert(a_move.from == KING_START(player))                           // king in starting square. Assert, as should be in bm
        // possibly add assert testing rook is there, should be handled by BM

        full_move.castle_bm[player] = GAME_CASTLE_NONE;
        full_move.special_bm |= GAME_SPECIAL_CASTLE;
    }
    else if ((p_a_board->pieces[a_move.from] == PIECE_TYPE_KING) && (a_move.to == SQ_RIGHT(SQ_RIGHT(a_move.from)))) // kingside castle
    {
        PRUNE(p_a_board->castle_bm[player] & GAME_CASTLE_KINGSIDE)          // make sure castle is legal  
        assert(a_move.from == KING_START(player))                           // king in starting square. Assert, as should be in bm
        PRUNE(p_a_board->pieces[SQ_LEFT(a_move.to)] == PIECE_TYPE_EMTPY)    // make sure F1/F8 is empty
        PRUNE(p_a_board->pieces[a_move.to] == PIECE_TYPE_EMTPY)             // make sure G1/G8 is empty (castles can't capture)
        // possibly add assert testing rook is there, should be handled by BM

        full_move.castle_bm[player] = GAME_CASTLE_NONE;
        full_move.special_bm |= GAME_SPECIAL_CASTLE;
    }
    else
    {
        PRUNE(_GAME_is_allowed_piece_movement(p_a_board, a_move))
    }

    if (is_capture) // if move is capture
    {
        PRUNE(p_a_board->colors[a_move.to] != player))           // can't capture own pieces
        full_move.special_bm |= GAME_SPECIAL_CAPTURE;
        full_move.capture = p_a_board->pieces[a_move.to]; 
    }

    switch (player) // finally, update castle bm
    {
    case WHITE:
        if (a_move.from == A1 || a_move.from == E1)
        {
            DISABLE_CASTLE(full_move.castle_bm[player], GAME_CASTLE_QUEENSIDE)
        }

        if (a_move.from == H1 || a_move.from == E1)
        {
            DISABLE_CASTLE(full_move.castle_bm[player], GAME_CASTLE_KINGSIDE)
        }
        break;
    case BLACK:
        if (a_move.from == A8 || a_move.from == E8)
        {
            DISABLE_CASTLE(full_move.castle_bm[player], GAME_CASTLE_QUEENSIDE)
        }

        if (a_move.from == H8 || a_move.from == E8)
        {
            DISABLE_CASTLE(full_move.castle_bm[player], GAME_CASTLE_KINGSIDE)
        }
        break;
    default:
        assert(0) // invalid color
    }

    return full_move;
}

GAME_board_t * GAME_new_board()
{
    // Allocate space
    GAME_board_t * p_board = (GAME_board_t*) malloc(sizeof(GAME_board_t));
    if (p_board == NULL)
    {
        return NULL;
    }
    
    // Initialize pieces and colors
    _GAME_init_pieces(p_board);
    _GAME_init_color(p_board);
    
    // Initialize parameters (except for history which will be trash at this point)
    p_board->turn         = 1;
    p_board->result       = GAME_RESULT_PLAYING;
    p_board->ep           = GAME_NO_EP;
    p_board->castle_bm[0] = GAME_CASTLE_ALL;
    p_board->castle_bm[1] = GAME_CASTLE_ALL;

    return p_board;
}

void GAME_free_board(GAME_board_t * p_a_board)
{
    if (p_a_board == NULL)
    {
        return;
    }

    free(p_a_board);
}

BOOL GAME_is_attacking(const GAME_board_t* p_a_board, COLOR color, square sq)
{
    // iterate over the squares
    for (int rank = 0; rank < NUM_RANKS; rank++)
    {
        for (int file = 0; file < NUM_FILES; file++)
        {
            square from = SQUARE_FROM_FILE_RANK(file, rank)
            // searching for all opponent pieces.
            if (GAME_current_player(board) == color))
            {
                assert(p_a_board->pieces[from] != PIECE_TYPE_EMPTY) // Having a color should suffice for a piece to be there.
                GAME_move_t move = { .from = from, .to = sq, .promote = PIECE_TYPE_QUEEN }; // Just to make sure we don't fall on invalid promotes
                if (_GAME_analayze_move(p_a_board, move) != NULL)  // if the move could theorethically be made (even leading to an illegal position)
                {
                    return TRUE; // return TRUE for check.
                }
            }
        }
    }

    return FALSE;
}

BOOL GAME_player_is_in_check(const GAME_board_t* p_a_board, COLOR color)
{
    // iterate over the squares
    for (int rank = 0; rank < NUM_RANKS; rank++)
    {
        for (int file = 0; file < NUM_FILES; file++)
        {
            // searching for the player king
            square to = SQUARE_FROM_FILE_RANK(file, rank)
            if (p_a_board->pieces[to] == PIECE_TYPE_KING && GAME_current_player(board) == color))
            {
                // and check if the opponent is attacking it.
                return GAME_is_attacking(p_a_board, OTHER_COLOR(color), to);
            }
        }
    }

    return FALSE;
}

GAME_MOVE_RESULTS_E GAME_make_move(GAME_board_t * p_a_board, GAME_move_t a_move)
{
    
    if (!SQ_IS_LEGAL(a_move.from)) // from square not on board.
    {
        return GAME_MOVE_RESULT_ILLEGALE_SQUARE;
    }
    if (p_a_board->colors[a_move.from] != current_player) // empty or opponent piece
    {
        return GAME_MOVE_RESULT_NO_PIECE;
    }

    GAME_full_move_t full_move = _GAME_analayze_move(p_a_board, a_move); 
    if (full_move == NULL)
    {
        return GAME_MOVE_RESULT_ILLEGAL_MOVE;
    }

    /* The move follows all movement rules, now to check if the move can be made without creating an illegal position */
    COLOR player   = GAME_get_current_player();
    COLOR opponent = OTHER_COLOR(player);

    // final castling checks (no check currently or in the way)
    if (full_move.special & GAME_SPECIAL_CASTLE) 
    {

        if (GAME_player_is_in_check(p_a_board, player)
        {
            return GAME_MOVE_RESULT_ILLEGAL_MOVE; // can't castle while in check.
        }
        square rook_from, rook_to;  // how to move the rook following the castle.

        // check the middle squares passed by the king is safe (the original and final squares are tested anyway)
        switch (full.move.to)
        {
        case C1:    // white queenside castle
            if (GAME_is_attacking(p_a_board, opponent, D1))
            {
                return GAME_MOVE_RESULT_ILLEGAL_MOVE
            }
            assert(p_a_board->pieces[A1] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = A1;
            rook_to = D1;
            break;
        case G1     // white kingside castle
            if (GAME_is_attacking(p_a_board, opponent, F1)) 
            {
                 return GAME_MOVE_RESULT_ILLEGAL_MOVE
            }
            assert(p_a_board->pieces[H1] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = H1;
            rook_to = F1;
            break;
        case C8     // black queenside castle
            if (GAME_is_attacking(p_a_board, opponent, D8)) 
            {
                 return GAME_MOVE_RESULT_ILLEGAL_MOVE
            }
            assert(p_a_board->pieces[A8] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = A8;
            rook_to = B8;
            break;
        case G8     // black kingside castle
            if (GAME_is_attacking(p_a_board, opponent, F8)) 
            {
                 return GAME_MOVE_RESULT_ILLEGAL_MOVE
            }
            assert(p_a_board->pieces[H8] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = H8;
            rook_to = F8;
            break;
        default:
            assert(0)   // castle flag was set but move wasn't castle.
        }

        p_a_board->pieces[rook_from] = PIECE_TYPE_EMPTY;
        p_a_board->color[rook_from]  = NO_COLOR;
    }

    memcpy(p_a_board->history[p_a_board->turn], full_move, sizeof(p_a_board->history[p_a_board->turn])); // Save move about to be made to history.
}


inline COLOR GAME_current_player(const GAME_board_t * p_a_board)
{
    return (p_a_board->turn & 1)
}

int main()
{
    GAME_board_t * p_board = GAME_new_board();
    printf("enum elem size: %lu\n", sizeof(PIECE_TYPE_PAWN));
    printf("history elem size: %lu\n", sizeof(GAME_move_full_t));
    printf("size: %lu\n", sizeof(GAME_board_t));

    for (int i=0; i < NUM_RANKS; i++)
    {
        for (int j=0; j < NUM_RANKS; j++)
        {
            printf("%d ",p_board->colors[SQ_FROM_FILE_RANK(j,i)]);
        }
        printf("\n");
    }
    printf("\n\n================\n\n");
    for (int i=0; i < NUM_RANKS; i++)
    {
        for (int j=0; j < NUM_RANKS; j++)
        {
            printf("%d ", p_board->pieces[SQ_FROM_FILE_RANK(j,i)]);
        }
        printf("\n");
    }
    printf("\n\n================\n\n");
    for (int i=0; i < NUM_RANKS; i++)
    {
        for (int j=0; j < NUM_RANKS; j++)
        {
            printf("%c ",PIECE_desc_lut[p_board->pieces[SQ_FROM_FILE_RANK(j,i)]].letters[p_board->colors[SQ_FROM_FILE_RANK(j,i)] & 1]);
        }
        printf("\n");
    }

    printf("color: %d other_color: %d not_color: %d other_not_color: %d\n",
            WHITE, OTHER_COLOR(WHITE), NO_COLOR, OTHER_COLOR(NO_COLOR));
    GAME_current_player(p_board);
    printf("%d\n", GAME_current_player(p_board));
}
