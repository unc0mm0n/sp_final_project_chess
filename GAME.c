#include <stdlib.h>

#include "GAME.h"

#include <stdlib.h>
#include <string.h>
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
        B, B, B, B, B, B, B, B, N, N, N, N, N, N, N, N,
        B, B, B, B, B, B, B, B, N, N, N, N, N, N, N, N,
        N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
        N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
        N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
        N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
        W, W, W, W, W, W, W, W, N, N, N, N, N, N, N, N,
        W, W, W, W, W, W, W, W, N, N, N, N, N, N, N, N
    };
    
    memcpy (p_board->colors, init_colors, sizeof(p_board->colors));
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
    p_board->turn = 1;
    p_board->result = GAME_RESULT_PLAYING;
    p_board->ep     = GAME_NO_EP;
    p_board->castle_bm = GAME_CASTLE_ALL;

    return p_board;
}

int main()
{
    GAME_board_t * p_board = GAME_new_board();
    printf("enum elem size: %lu\n", sizeof(PIECE_TYPE_PAWN));
    printf("history elem size: %lu\n", sizeof(GAME_move_full_t));
    printf("size: %lu\n", sizeof(GAME_board_t));
    p_board->castle_bm = DISABLE_CASTLE(p_board->castle_bm, GAME_CASTLE_B_KINGSIDE);
    printf("0x%x\n", p_board->castle_bm);

    for (int i=0; i < RANK_NUM; i++)
    {
        for (int j=0; j < RANK_NUM; j++)
        {
            printf("%d ",p_board->colors[SQUARE_FROM_FILE_RANK(j,i)]);
        }
        printf("\n");
    }
    printf("\n\n================\n\n");
    for (int i=0; i < RANK_NUM; i++)
    {
        for (int j=0; j < RANK_NUM; j++)
        {
            printf("%d ", p_board->pieces[SQUARE_FROM_FILE_RANK(j,i)]);
        }
        printf("\n");
    }
    printf("\n\n================\n\n");
    for (int i=0; i < RANK_NUM; i++)
    {
        for (int j=0; j < RANK_NUM; j++)
        {
            printf("%c ",PIECE_desc_lut[p_board->pieces[SQUARE_FROM_FILE_RANK(j,i)]].letters[p_board->colors[SQUARE_FROM_FILE_RANK(j,i)] & 1]);
        }
        printf("\n");
    }

    printf("color: %d other_color: %d not_color: %d other_not_color: %d\n",
            WHITE, OTHER_COLOR(WHITE), NO_COLOR, OTHER_COLOR(NO_COLOR));
    GAME_current_player(p_board);
    printf("%d\n", GAME_current_player(p_board));
}
