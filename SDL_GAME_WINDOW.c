#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SDL_GAME_WINDOW.h"
#include "PIECE.h"
#include "GAME.h"

square _SDL_GAME_WINDOW_get_sq_from_x_y(int x, int y)
{
    return SQ_FROM_FILE_RANK( (x - BOARD_OFFSET_H) / CELL_SIZE, 7 - (y / CELL_SIZE));
}

SDL_Texture* SDL_INTERFACE_load_texture_from_bmp(const char* filename, SDL_Renderer* renderer, BOOL use_color_key)
{
    if (filename == NULL)
    {
        return NULL;
    }
    SDL_Texture* p_texture;

    SDL_Surface* loadingSurface = SDL_LoadBMP(filename);
    if (loadingSurface == NULL ) {
        printf("Could not create a surface: %s\n", SDL_GetError());
        return NULL;
    }

    if (use_color_key)
    {
        SDL_SetColorKey(loadingSurface, SDL_TRUE, SDL_MapRGB(loadingSurface->format, 0xFF, 0xFF, 0xFF));
    }

    p_texture = SDL_CreateTextureFromSurface(renderer, loadingSurface);
    if (p_texture == NULL ) {
        printf("Could not create a texture: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(loadingSurface); //We finished with the surface -> delete it
    return p_texture;
}

SDL_GAME_WINDOW_view_t* SDL_GAME_WINDOW_create_view()
{
    SDL_GAME_WINDOW_view_t* p_view = malloc(sizeof(SDL_GAME_WINDOW_view_t));
    SDL_Window* window = SDL_CreateWindow("Tests", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);
    if (p_view == NULL || window == NULL || renderer == NULL) 
    {
        free(p_view);
        //We first destroy the renderer
        SDL_DestroyRenderer(renderer); //NULL safe
        SDL_DestroyWindow(window); //NULL safe
        return NULL ;
    }

    p_view->window = window;
    p_view->renderer = renderer;
    p_view->bg_texture = SDL_INTERFACE_load_texture_from_bmp("./graphics/bg.bmp", renderer, FALSE);
    p_view->pieces_texture_white = malloc(sizeof(SDL_Texture*) * PIECE_TYPE_MAX);
    p_view->pieces_texture_black = malloc(sizeof(SDL_Texture*) * PIECE_TYPE_MAX);

    p_view->active_sq = GAME_WINDOW_NO_ACTIVE_SQ;
    p_view->marked_moves = NULL;
    p_view->fixed_castle = TRUE;

    char wname[255];
    char bname[255];
    for (int i = PIECE_TYPE_PAWN; i <= PIECE_TYPE_KING; i++)
    {
        wname[0] = '\0';
        strcat(wname, "./graphics/W");
        strcat(wname, PIECE_desc_lut[i].name);
        strcat(wname, ".bmp");
        p_view->pieces_texture_white[i] = SDL_INTERFACE_load_texture_from_bmp(wname, renderer, TRUE);

        bname[0] = '\0';
        strcat(bname, "./graphics/B");
        strcat(bname, PIECE_desc_lut[i].name);
        strcat(bname, ".bmp");

        p_view->pieces_texture_black[i] = SDL_INTERFACE_load_texture_from_bmp(bname, renderer, TRUE);
    }

    p_view->buttons = malloc(sizeof(SDL_button_t) * GAME_WINDOW_MAX_BUTTONS);
    p_view->button_count = 0;

    SDL_GAME_WINDOW_add_button(p_view, "./graphics/newgame.bmp", NULL, NULL); 
    SDL_GAME_WINDOW_add_button(p_view, "./graphics/loadgame.bmp", NULL, NULL); 
    SDL_GAME_WINDOW_add_button(p_view, "./graphics/savegame.bmp", NULL, NULL); 
    SDL_GAME_WINDOW_add_button(p_view, "./graphics/mainmenu.bmp", NULL, NULL); 
    SDL_GAME_WINDOW_add_button(p_view, "./graphics/quit.bmp", NULL, NULL); 
    SDL_GAME_WINDOW_add_button(p_view, "./graphics/bg.bmp", NULL, NULL); 
    return p_view;
}

void SDL_GAME_WINDOW_add_button(SDL_GAME_WINDOW_view_t* p_view, const char* active_texture_fn, const char* inactive_texture_fn, MANAGER_agent_command_t (*cb)())
{
    assert(p_view->button_count < GAME_WINDOW_MAX_BUTTONS);
    SDL_Rect location = {.x=BUTTON_X, .y=BUTTON_PADDING + p_view->button_count * BUTTON_AREA_HEIGHT, .h=BUTTON_HEIGHT, .w=BUTTON_WIDTH};
    SDL_Texture * b_texture = SDL_INTERFACE_load_texture_from_bmp(active_texture_fn, p_view->renderer, FALSE);
    SDL_Texture* d_texture = SDL_INTERFACE_load_texture_from_bmp(inactive_texture_fn, p_view->renderer, FALSE);
    p_view->buttons[p_view->button_count] = SDL_BUTTON_create(TRUE, cb, b_texture, d_texture, location);
    p_view->button_count++;
}

void SDL_GAME_WINDOW_destroy_view(SDL_GAME_WINDOW_view_t* p_view)
{
    free(p_view->pieces_texture_white);
    free(p_view->pieces_texture_black);

    // All textures are destoyed by renderer
    SDL_DestroyRenderer(p_view->renderer);
    SDL_DestroyWindow(p_view->window);

    for (int i = 0 ; i < p_view->button_count; i++)
    {
        SDL_BUTTON_destroy(p_view->buttons[i]);
    }
    free(p_view->buttons);

    free(p_view);
}

void SDL_GAME_WINDOW_draw_view(SDL_GAME_WINDOW_view_t* p_view, const GAME_board_t* p_board)
{
    if(p_view == NULL){
        return;
    }
    SDL_Rect rec = { .x = 200, .y = 0, .w = 600, .h = 600 };
    SDL_SetRenderDrawColor(p_view->renderer, 150, 150, 150, 255);
    SDL_RenderClear(p_view->renderer);
    SDL_RenderCopy(p_view->renderer, p_view->bg_texture, NULL, &rec);

    rec.w = CELL_SIZE;
    rec.h = CELL_SIZE;
    GAME_move_analysis_t* tmp;
    tmp = p_view->marked_moves;
    while (!p_view->fixed_castle && tmp != NULL && tmp->verdict == GAME_MOVE_VERDICT_LEGAL) // fix castle moves in the mix
    {
        if (tmp->special_bm & GAME_SPECIAL_CASTLE)
        {
            if (tmp->move.to == SQ_LEFT(SQ_LEFT(tmp->move.from))) // queenside castle
            {
                tmp->move.to = SQ_LEFT(SQ_LEFT(tmp->move.to));
            }
            else
            {
                tmp->move.to = SQ_RIGHT(tmp->move.to);
            }
        }
        tmp++;
    }
    p_view->fixed_castle = TRUE;
    for (int rank = 0; rank < NUM_RANKS; rank++)
    {
        for (int file = 0; file < NUM_FILES; file++)
        {
            rec.x = BOARD_OFFSET_H + file * CELL_SIZE;
            rec.y = (7 - rank) * CELL_SIZE;

            square sq = SQ_FROM_FILE_RANK(file,rank);
            int sq_col = 50 + 155 * ((rank + file) & 1);

            tmp = p_view->marked_moves;
            while (tmp != NULL && tmp->verdict == GAME_MOVE_VERDICT_LEGAL)
            {
                if (sq == tmp->move.to)
                {
                    if ((tmp->special_bm & GAME_SPECIAL_CASTLE) > 0)
                    {
                        sq_col = 0x0;
                    }
                    else
                    {
                        sq_col = 0xFF;
                    }
                    break;
                }
                tmp++;
            }
            if (sq == p_view->active_sq)
            {
                SDL_SetRenderDrawColor(p_view->renderer, GAME_WINDOW_ACTIVE_SQ_COLOR, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(p_view->renderer, sq_col, sq_col, sq_col, 255);
            }
            int color = p_board->colors[sq];

            SDL_RenderFillRect(p_view->renderer, &rec);
            PIECE_TYPE_E piece = p_board->pieces[sq];
            switch (color)
            {
                case BLACK:
                    {
                        SDL_RenderCopy(p_view->renderer, p_view->pieces_texture_black[piece], NULL, &rec);
                        break;
                    }
                case WHITE:
                    {
                        SDL_RenderCopy(p_view->renderer, p_view->pieces_texture_white[piece], NULL, &rec);
                        break;
                    }
                default:
                    break;
            }
        }
    }

    for (int i=0; i < p_view->button_count; i++)
    {
        SDL_BUTTON_render(p_view->buttons[i], p_view->renderer);
    }    
    SDL_RenderPresent(p_view->renderer);
}

MANAGER_agent_play_command_t SDL_GAME_WINDOW_handle_event(SDL_GAME_WINDOW_view_t* p_view, SDL_Event* event, const GAME_board_t* p_board)
{
    MANAGER_agent_play_command_t cmd;
    cmd.type = MANAGER_PLAY_COMMAND_TYPE_NONE;
    if(event->type == SDL_MOUSEBUTTONUP) //SDL_MouseButtonEvent
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (event->button.x > BOARD_OFFSET_H) // board click
            {
                square sq = _SDL_GAME_WINDOW_get_sq_from_x_y(event->button.x, event->button.y);
                if (sq != p_view->active_sq)
                {
                    if (p_board->pieces[p_view->active_sq] == PIECE_TYPE_KING && 
                            (abs(SQ_TO_FILE(sq) - SQ_TO_FILE(p_view->active_sq)) > 1))
                    { // king moving more than one square, can only be legal if it's castle
                        cmd.type = MANAGER_PLAY_COMMAND_TYPE_CASTLE;
                        cmd.data.sq = sq;
                    }
                    else if (p_board->pieces[sq] == PIECE_TYPE_KING)
                    { // king "capture" is only possible if trying to castle
                        cmd.type = MANAGER_PLAY_COMMAND_TYPE_CASTLE;
                        cmd.data.sq = p_view->active_sq;
                    }
                    else // a normal move
                    {
                        cmd.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;
                        GAME_move_t move = {.from = p_view->active_sq, .to = sq, .promote = PIECE_TYPE_QUEEN};

                        cmd.data.move = move;
                    }
                }
                p_view->active_sq = GAME_WINDOW_NO_ACTIVE_SQ;
            }
            else // button click, or empty space
            {
                for (int i=0; i < p_view->button_count; i++)
                {
                    MANAGER_agent_command_t c = SDL_BUTTON_handle_event(p_view->buttons[i], event);
                    if (c.type != MANAGER_COMMAND_TYPE_INVALID)
                    {
                        assert(c.type == MANAGER_COMMAND_TYPE_PLAY_COMMAND);
                        return c.cmd.play_command;
                    }
                }
            }
            p_view->active_sq = GAME_WINDOW_NO_ACTIVE_SQ;
        }
        p_view->marked_moves = NULL;
        p_view->fixed_castle = FALSE;
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (event->button.x > BOARD_OFFSET_H) // board click
            {
                square sq = _SDL_GAME_WINDOW_get_sq_from_x_y(event->button.x, event->button.y);
                if (p_board->colors[sq] == GAME_current_player(p_board))
                {
                    p_view->active_sq = sq;
                }
            }
        }
        square sq = _SDL_GAME_WINDOW_get_sq_from_x_y(event->button.x, event->button.y);
        cmd.type = MANAGER_PLAY_COMMAND_TYPE_GET_MOVES;
        cmd.data.sq = sq;
    }
    return cmd;
}

