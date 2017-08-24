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
        B,B,B,B,B,B,B,B, N,N,N,N,N,N,N,N,
        B,B,B,B,B,B,B,B, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N,N,N, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N,N,N, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N,N,N, N,N,N,N,N,N,N,N,
        N,N,N,N,N,N,N,N, N,N,N,N,N,N,N,N,
        W,W,W,W,W,W,W,W, N,N,N,N,N,N,N,N,
        W,W,W,W,W,W,W,W, N,N,N,N,N,N,N,N
    };
    
    memcpy (p_board->colors, init_colors, sizeof(p_board->colors));
}

//#define PRUNE(cond,move) if(!(cond)){(move).valid = FALSE; return (move);}   // helper local define to prune moves.
#define PRUNE(cond,move) assert(cond); // debug prune

/**
 * Return true if the piece can theoretically move there on an 
 * empty board. 
 * As always does not handle the case of pawns. 
 *  
 * Here is the main point for which we we need the LUT defined 
 * in PIECES. 
 */
BOOL _GAME_is_allowed_piece_movement(const GAME_board_t* p_a_board, GAME_move_t a_move)
{
    PIECE_TYPE_E piece = p_a_board->pieces[a_move.from];
    PIECE_desc_t piece_desc = PIECE_desc_lut[piece];
    for (int i = 0; i < PIECE_MAX_OFFSETS; i++)
    {
        if (piece_desc.offsets[i] == PIECE_OFFSET_SENTINEL)
        {
            return FALSE; // We have checked all piece offsets and haven't found a match.
        }

        square tmp = a_move.to; // currently looked at square.
        int offset = piece_desc.offsets[i]; // offset to move the piece at each step.

        while (SQ_IS_LEGAL(tmp)) 
        {
            tmp += offset;
            if (tmp == a_move.from)
            {
                return TRUE;        // We have found a move sequence leading to the destination.
            }
            if (p_a_board->colors[tmp] != NO_COLOR)
            {
                break;              // path is blocked by something
            }
            if (!piece_desc.slides) // Can only move once in this offset, switch to next offset.
            {
                break;
            }      
        }

    }

    return FALSE;
}

/**
 * Save move and all current game parameters to history, so that 
 * we can undo it if necessary. 
 *  
 * The history is saved on the game's current turn 
 */
void _GAME_save_history(GAME_board_t *p_a_board, GAME_move_full_t move)
{
    int turn = p_a_board->turn;
    p_a_board->history[turn].move = move; 
    memcpy(p_a_board->history[turn].castle_bm, p_a_board->castle_bm, sizeof(p_a_board->history[turn].castle_bm)); 
    p_a_board->history[turn].ep = p_a_board->ep; 
}

/**
 * Update castle bitmask according to if the king or rook moved. 
 * Color is not important as if an opponent piece moved there or
 * from there, our original piece wasn't there. 
 */

void _GAME_update_castle_bm(int * p_a_castle_bm, GAME_move_t a_move)
{
    if (a_move.from == A1 || a_move.to == A1 || a_move.from == E1) // we do not need to check move to E1, as king can't be captured
    {
        p_a_castle_bm[WHITE] = DISABLE_CASTLE(p_a_castle_bm[WHITE], GAME_CASTLE_QUEENSIDE);
    }

    if (a_move.from == H1 || a_move.to == H1 || a_move.from == E1)
    {
        p_a_castle_bm[WHITE] = DISABLE_CASTLE(p_a_castle_bm[WHITE], GAME_CASTLE_KINGSIDE);
    }
    if (a_move.from == A8 || a_move.to == A8 || a_move.from == E8)
    {
        p_a_castle_bm[BLACK] = DISABLE_CASTLE(p_a_castle_bm[BLACK], GAME_CASTLE_QUEENSIDE);
    }

    if (a_move.from == H8 || a_move.to == H8 || a_move.from == E8)
    {
        p_a_castle_bm[BLACK] = DISABLE_CASTLE(p_a_castle_bm[BLACK], GAME_CASTLE_KINGSIDE);
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
    
    full_move.valid = TRUE;
    full_move.special_bm = 0;
    full_move.move = a_move;

    BOOL is_capture = (p_a_board->colors[a_move.to] == OTHER_COLOR(player));

    if (p_a_board->pieces[a_move.from] == PIECE_TYPE_PAWN) // Pawns get special treatment
    {
        BOOL is_move;
        BOOL is_double_move;
        BOOL is_diag_move;
        BOOL is_ep = (a_move.to == p_a_board->ep); // Is en passant capture
        switch (player)
        {
        case WHITE:
            is_move = (a_move.to == SQ_UP(a_move.from));                                                        // move up
            is_double_move = (IS_WHITE_PAWN_RANK(a_move.from)) && (a_move.to == SQ_UP(SQ_UP(a_move.from)));     // or move twice up from initial square
            is_diag_move = SQ_IS_DIAG_UP(a_move.from, a_move.to);                                               // or move diagonally if capture or ep
            break;
        case BLACK:
            is_move = (a_move.to == SQ_DOWN(a_move.from));                                                      // move down
            is_double_move = (IS_BLACK_PAWN_RANK(a_move.from)) && (a_move.to == SQ_DOWN(SQ_DOWN(a_move.from))); // or move twice down from initial square
            is_diag_move = SQ_IS_DIAG_DOWN(a_move.from, a_move.to);                                             // or move diagonally if capture or ep
            break;
        default:
            assert(0); // invalid color
        }

        PRUNE(is_move || is_double_move || (is_diag_move && (is_capture || is_ep)), full_move);

        if (SQ_TO_RANK(a_move.to) == LAST_RANK(player)) // promotion
        {
            PRUNE(PIECE_desc_lut[a_move.promote].can_promote_to, full_move);

            full_move.special_bm |= GAME_SPECIAL_PROMOTE;
        }

        if (is_ep) // en passant capture
        {
            full_move.special_bm |= GAME_SPECIAL_EP_CAPTURE;
        }

        if (is_double_move) // to update ep is possible.
        {
            full_move.special_bm |= GAME_SPECIAL_PAWN_DOUBLE_MOVE;
        }
    }
    else if ((p_a_board->pieces[a_move.from] == PIECE_TYPE_KING) && (a_move.to == SQ_LEFT(SQ_LEFT(a_move.from))))   // queenside castle
    {
        PRUNE(p_a_board->castle_bm[player] & GAME_CASTLE_QUEENSIDE, full_move)         // make sure castle is allowed
        PRUNE(p_a_board->pieces[SQ_LEFT(a_move.to)] == PIECE_TYPE_EMPTY, full_move)    // make sure B1/B8 is empty
        PRUNE(p_a_board->pieces[a_move.to] == PIECE_TYPE_EMPTY, full_move)             // make sure C1/C8 is empty (castles can't capture)
        PRUNE(p_a_board->pieces[SQ_RIGHT(a_move.to)] == PIECE_TYPE_EMPTY, full_move)   // make sure D1/D8 is empty
        assert(a_move.from == KING_START(player));                                     // king in starting square. Assert, as should be in bm

        full_move.special_bm |= GAME_SPECIAL_CASTLE;
    }
    else if ((p_a_board->pieces[a_move.from] == PIECE_TYPE_KING) && (a_move.to == SQ_RIGHT(SQ_RIGHT(a_move.from)))) // kingside castle
    {
        PRUNE(p_a_board->castle_bm[player] & GAME_CASTLE_KINGSIDE, full_move)          // make sure castle is allowed  
        assert(a_move.from == KING_START(player));                                     // king in starting square. Assert, as should be in bm
        PRUNE(p_a_board->pieces[SQ_LEFT(a_move.to)] == PIECE_TYPE_EMPTY, full_move)    // make sure F1/F8 is empty
        PRUNE(p_a_board->pieces[a_move.to] == PIECE_TYPE_EMPTY, full_move)             // make sure G1/G8 is empty (castles can't capture)
        // possibly add assert testing rook is there, should be handled by BM

        full_move.special_bm |= GAME_SPECIAL_CASTLE;
    }
    else
    {
        PRUNE(_GAME_is_allowed_piece_movement(p_a_board, a_move), full_move)
    }

    if (is_capture) // if move is capture
    {
        PRUNE(p_a_board->colors[a_move.to] != player, full_move)           // can't capture own pieces
        assert(p_a_board->pieces[a_move.to] != PIECE_TYPE_KING);           // a position in which capturing the king is possible is not legal
        full_move.special_bm |= GAME_SPECIAL_CAPTURE;
        full_move.capture = p_a_board->pieces[a_move.to]; 
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
    
    // Initialize parameters
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
    /* We test if a square is attacked by testing if replacing it by some piece could take an opponent piece of the same type.
       This is similar to yet different from the reachibility test done above. */

    if (!IS_COLOR(color))
    {
        return FALSE;
    }

    // Check if pawn attacks
    square sq_l, sq_r;
    if (color == WHITE)
    {
        sq_l = SQ_DOWN_LEFT(sq);
        sq_r = SQ_DOWN_RIGHT(sq);
    }
    else
    {
        sq_l = SQ_UP_LEFT(sq);
        sq_r = SQ_UP_RIGHT(sq);
    }

    if (p_a_board->pieces[sq_l] == PIECE_TYPE_PAWN && p_a_board->colors[sq_l] == color) 
    {
        return TRUE;
    }
    if (p_a_board->pieces[sq_r] == PIECE_TYPE_PAWN && p_a_board->colors[sq_r] == color) 
    {
        return TRUE;
    }

    PIECE_desc_t desc;
    // check other pieces
    for (int i = PIECE_TYPE_KNIGHT; i < PIECE_TYPE_MAX; i++)
    {
        desc = PIECE_desc_lut[i];
        square tmp;
        int offset;

        for (int j = 0; j < PIECE_MAX_OFFSETS; j++)
        {
            if (desc.offsets[j] == PIECE_OFFSET_SENTINEL)
            {
                break; // We have checked all piece offsets and haven't found a match.
            }
            tmp = sq;
            offset = desc.offsets[j]; // offset to move the piece at each step.
            tmp += offset;

            while (SQ_IS_LEGAL(tmp))
            {
                if (p_a_board->pieces[tmp] == i && p_a_board->colors[tmp] == color)
                {
                    return TRUE;        // We have found a piece which threatens the square
                }
                if (p_a_board->colors[tmp] != NO_COLOR)
                {
                    break;        // Path is blocked by a piece which can't capture.
                }
                if (!desc.slides) // Can only move once in this offset, switch to next offset.
                {
                    break;
                }
                tmp += offset;
            }
        }
    }

    return FALSE;
}

BOOL GAME_is_checked(const GAME_board_t* p_a_board, COLOR color)
{
    // iterate over the squares
    for (int rank = 0; rank < NUM_RANKS; rank++)
    {
        for (int file = 0; file < NUM_FILES; file++)
        {
            // searching for the player king
            square to = SQ_FROM_FILE_RANK(file, rank);
            if (p_a_board->pieces[to] == PIECE_TYPE_KING && p_a_board->colors[to]  == color)
            {
                printf("Player %d king found at %x\n", color, to);
                // and check if the opponent is attacking it.
                return GAME_is_attacking(p_a_board, OTHER_COLOR(color), to);
            }
        }
    }

    return FALSE;
}

GAME_MOVE_RESULTS_E GAME_make_move(GAME_board_t * p_a_board, GAME_move_t a_move)
{
    
    COLOR player = GAME_current_player(p_a_board);

    if (!SQ_IS_LEGAL(a_move.from)) // from square not on board.
    {
        return GAME_MOVE_RESULT_ILLEGALE_SQUARE;
    }
    printf("playing: %d %d %d %d\n", player, a_move.from, a_move.to, p_a_board->colors[a_move.from]);
    if (p_a_board->colors[a_move.from] != player) // empty or opponent piece
    {
        return GAME_MOVE_RESULT_NO_PIECE;
    }

    GAME_move_full_t full_move = _GAME_analayze_move(p_a_board, a_move); 
    if (!full_move.valid)
    {
        return GAME_MOVE_RESULT_ILLEGAL_MOVE;
    }

    /* The move follows all movement rules, now to check if the move can be made without creating an illegal position */
    COLOR opponent = OTHER_COLOR(player);

    // final castling checks (no check currently or in the way)
    if (full_move.special_bm & GAME_SPECIAL_CASTLE) 
    {
        if (GAME_is_checked(p_a_board, player))
        {
            return GAME_MOVE_RESULT_ILLEGAL_MOVE; // can't castle while in check.
        }
        square rook_from, rook_to;  // how to move the rook following the castle.
        // check the middle squares passed by the king is safe (the original and final squares are tested anyway)
        switch (full_move.move.to)
        {
        case C1:    // white queenside castle
            if (GAME_is_attacking(p_a_board, opponent, D1))
            {
                return GAME_MOVE_RESULT_ILLEGAL_MOVE;
            }
            assert(p_a_board->pieces[A1] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = A1;
            rook_to = D1;
            break;
        case G1:     // white kingside castle
            if (GAME_is_attacking(p_a_board, opponent, F1)) 
            {
                 return GAME_MOVE_RESULT_ILLEGAL_MOVE;
            }
            assert(p_a_board->pieces[H1] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = H1;
            rook_to = F1;
            break;
        case C8:     // black queenside castle
            if (GAME_is_attacking(p_a_board, opponent, D8)) 
            {
                 return GAME_MOVE_RESULT_ILLEGAL_MOVE;
            }
            assert(p_a_board->pieces[A8] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = A8;
            rook_to = D8;
            break;
        case G8:     // black kingside castle
            if (GAME_is_attacking(p_a_board, opponent, F8)) 
            {
                 return GAME_MOVE_RESULT_ILLEGAL_MOVE;
            }
            assert(p_a_board->pieces[H8] == PIECE_TYPE_ROOK); // should have been handled long ago by bitmask
            rook_from = H8;
            rook_to = F8;
            break;
        default:
            assert(0);   // castle flag was set but move wasn't castle.
        }
        // update rook position to after castle
        p_a_board->pieces[rook_from] = PIECE_TYPE_EMPTY;
        p_a_board->colors[rook_from]  = NO_COLOR;
        p_a_board->pieces[rook_to]   = PIECE_TYPE_ROOK;
        p_a_board->colors[rook_to]    = player;
    }

    // save in history, in case of undo.
    _GAME_save_history(p_a_board, full_move);
    // update game parameters
    _GAME_update_castle_bm(p_a_board->castle_bm, full_move.move);
    if (full_move.special_bm & GAME_SPECIAL_PAWN_DOUBLE_MOVE) // update ep is possible
    {
        if (player == WHITE)
        {
            p_a_board->ep = SQ_DOWN(full_move.move.to);
        }
        else
        {
            p_a_board->ep = SQ_UP(full_move.move.to);
        }
    }
    else // or not possible
    {
        p_a_board->ep = GAME_NO_EP;
    }
    p_a_board->turn++;
    
    // move piece
    p_a_board->colors[full_move.move.to] = player;
    p_a_board->colors[full_move.move.from] = NO_COLOR;

    if (full_move.special_bm & GAME_SPECIAL_PROMOTE) // if promotion switch to promoted piece
    {
        p_a_board->pieces[full_move.move.to] = full_move.move.promote; 
    }
    else
    {
        p_a_board->pieces[full_move.move.to] = p_a_board->pieces[full_move.move.from]; 
    }
    p_a_board->pieces[full_move.move.from] = PIECE_TYPE_EMPTY;

    if (full_move.special_bm & GAME_SPECIAL_EP_CAPTURE)  // if we took enpassant, remove the pawn
    {
        if (player == WHITE)
        {
            p_a_board->pieces[SQ_DOWN(full_move.move.to)] = PIECE_TYPE_EMPTY;
        }
        else
        {
            p_a_board->pieces[SQ_UP(full_move.move.to)] = PIECE_TYPE_EMPTY;
        }
    }

    // finally we can check if the move leads to a legal position
    if (GAME_is_checked(p_a_board, player))
    {
        printf("king was checked, undoing");
        GAME_undo_move(p_a_board); // All this was for naught.
        return GAME_MOVE_RESULT_ILLEGAL_MOVE;
    }

    // and update final parameters in the move history.
    printf("Current player: %d, other player: %d, is_in_check: %d, is_attacking: %d\n", GAME_current_player(p_a_board), OTHER_COLOR(player), GAME_is_checked(p_a_board, OTHER_COLOR(player)), GAME_is_attacking(p_a_board, OTHER_COLOR(player), a_move.to));
    printf("Is attacking g8: %d\n", GAME_is_attacking(p_a_board, player, G8));
    p_a_board->history[p_a_board->turn - 1].move.special_bm |= GAME_SPECIAL_CHECK * GAME_is_checked(p_a_board, OTHER_COLOR(player)); 
    p_a_board->history[p_a_board->turn - 1].move.special_bm |= GAME_SPECIAL_UNDER_ATTACK * GAME_is_attacking(p_a_board, OTHER_COLOR(player), a_move.to);

    return GAME_MOVE_RESULT_SUCCESS;
}

GAME_move_full_t GAME_undo_move(GAME_board_t * p_a_board)
{
    /* this is really similar to a normal move, but in reverse and without legality checking. */
    p_a_board->turn--;  // update turn
    GAME_history_t hist = p_a_board->history[p_a_board->turn]; // fetch history

    // update parameters
    memcpy(p_a_board->castle_bm, hist.castle_bm, sizeof(p_a_board->castle_bm));
    p_a_board->ep = hist.ep;

    /* reverse move */
    COLOR moving_player = GAME_current_player(p_a_board);
    GAME_move_full_t last_move = hist.move;

    // reverse ep
    if (last_move.special_bm & GAME_SPECIAL_EP_CAPTURE) 
    {
        p_a_board->pieces[hist.ep] = PIECE_TYPE_PAWN;
        p_a_board->colors[hist.ep] = OTHER_COLOR(moving_player);
    }

     // move piece back
    p_a_board->colors[last_move.move.from] = moving_player;
    p_a_board->colors[last_move.move.to] = NO_COLOR;

    if (last_move.special_bm & GAME_SPECIAL_PROMOTE) // if promotion switch back to pawn
    {
        p_a_board->pieces[last_move.move.from] = PIECE_TYPE_PAWN; 
    }
    else // otherwise keep original piece
    {
        p_a_board->pieces[last_move.move.from] = p_a_board->pieces[last_move.move.to]; 
    }
    p_a_board->pieces[last_move.move.to] = PIECE_TYPE_EMPTY;

    // reverse castling
    if (last_move.special_bm & GAME_SPECIAL_CASTLE)
    {
        square rook_from, rook_to;  // how to move the rook to before the castle
        switch (last_move.move.to)
        {
        case C1:    // white queenside castle
            assert(p_a_board->pieces[D1] == PIECE_TYPE_ROOK); // if just castled, should be rook there
            rook_from = D1;
            rook_to = A1;
            break;
        case G1:     // white kingside castle
            assert(p_a_board->pieces[F1] == PIECE_TYPE_ROOK); // if just castled, should be rook there
            rook_from = F1;
            rook_to = H1;
            break;
        case C8:     // black queenside castle
            assert(p_a_board->pieces[D8] == PIECE_TYPE_ROOK); // if just castled, should be rook there
            rook_from = D8;
            rook_to = A8;
            break;
        case G8:     // black kingside castle
            assert(p_a_board->pieces[F8] == PIECE_TYPE_ROOK); // if just castled, should be rook there
            rook_from = F8;
            rook_to = H8;
            break;
        default:
            assert(0);   // castle flag was set but move wasn't castle.
        }
        // update rook position to after castle
        p_a_board->pieces[rook_from] = PIECE_TYPE_EMPTY;
        p_a_board->colors[rook_from]  = NO_COLOR;
        p_a_board->pieces[rook_to]   = PIECE_TYPE_ROOK;
        p_a_board->colors[rook_to]    = moving_player;
    }

    return last_move;
}


COLOR GAME_current_player(const GAME_board_t * p_a_board)
{
    return (p_a_board->turn & 1);
}

void CLI_print_board(const GAME_board_t * p_a_board)
{
    for (int i=NUM_RANKS-1; i >= 0; i--)
    {
        for (int j=0; j < NUM_RANKS; j++)
        {
            printf("%c ",PIECE_desc_lut[p_a_board->pieces[SQ_FROM_FILE_RANK(j,i)]].letters[p_a_board->colors[SQ_FROM_FILE_RANK(j,i)] & 1]);
        }
        printf("\n");
    }
}

/** Test function with more convenient notations */
void _GAME_test_play(GAME_board_t* p_board, int ff, int fr, int tf, int tr)
{
    printf("\n\n=================\n\nplayed e4?\n");
    printf("Playing %d-%d to %d-%d\n", ff, fr, tf, tr);
    GAME_move_t m = {.from=SQ_FROM_FILE_RANK(ff-1,fr-1),.to=SQ_FROM_FILE_RANK(tf-1,tr-1),.promote=PIECE_TYPE_QUEEN };
    printf("square: from %x, to %x, promote %d\n", m.from, m.to, m.promote);
    printf("move result %d\n", GAME_make_move(p_board, m));
    CLI_print_board(p_board);
    GAME_move_full_t lm = p_board->history[p_board->turn-1].move;
    printf("special_bm: %x castle_bm_w: %x castle_bm_b: %x ep: %x turn:%d \n", lm.special_bm, p_board->castle_bm[WHITE], p_board->castle_bm[BLACK], p_board->ep, p_board->turn);
    printf("current player %d\n", GAME_current_player(p_board));
}

int main()
{
    GAME_board_t * p_board = GAME_new_board();
    printf("enum elem size: %lu\n", sizeof(PIECE_TYPE_PAWN));
    printf("history elem size: %lu\n", sizeof(GAME_history_t));
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
    CLI_print_board(p_board);

    printf("color: %d other_color: %d not_color: %d other_not_color: %d\n",
            WHITE, OTHER_COLOR(WHITE), NO_COLOR, OTHER_COLOR(NO_COLOR));

    /* check and pinned piece test */
     
    _GAME_test_play(p_board,5,2,5,4); //e4
    _GAME_test_play(p_board,5,7,5,5); //e5
    _GAME_test_play(p_board,4,2,4,3); //d3
    _GAME_test_play(p_board,6,8,2,4); //Bd4+
    _GAME_test_play(p_board,1,2,1,3); //a3 (illegal)
    _GAME_test_play(p_board,2,1,3,3); //Nc3
    _GAME_test_play(p_board,4,7,4,5); //d5
    _GAME_test_play(p_board,3,3,4,5); //Nxd5 (illegal)
    /* Enpassant casstle basic check test
    _GAME_test_play(p_board, 5,2,5,4);
    _GAME_test_play(p_board, 5,2,5,4);
    _GAME_test_play(p_board, 5,7,5,5);
    _GAME_test_play(p_board, 4,2,4,4);
    _GAME_test_play(p_board, 5,5,4,4);
    _GAME_test_play(p_board, 3,2,3,4);
    _GAME_test_play(p_board, 4,4,3,3);
    _GAME_test_play(p_board, 2,1,3,3);
    _GAME_test_play(p_board, 7,8,6,6);
    _GAME_test_play(p_board, 5,1,5,2);
    _GAME_test_play(p_board, 6,8,2,4);
    _GAME_test_play(p_board, 5,4,5,5);
    _GAME_test_play(p_board, 5,8,7,8);
    _GAME_test_play(p_board, 5,5,5,6);
    _GAME_test_play(p_board, 2,8,3,6);
    _GAME_test_play(p_board, 5,6,5,7);
    _GAME_test_play(p_board, 3,6,5,5);
    _GAME_test_play(p_board, 5,7,6,8);
    _GAME_test_play(p_board, 7,7,7,6);
    _GAME_test_play(p_board, 6,8,7,8);
    */
}
