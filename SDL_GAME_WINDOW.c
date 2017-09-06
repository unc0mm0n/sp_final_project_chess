#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SDL_GAME_WINDOW.h"
#include "PIECE.h"
#include "GAME.h"

SDL_Texture* SDL_INTERFACE_load_texture_from_bmp(const char* filename, SDL_Renderer* renderer, BOOL use_color_key)
{
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
   
   SDL_GAME_WINDOW_add_button(p_view, "./graphics/newgame.bmp", NULL); 
   SDL_GAME_WINDOW_add_button(p_view, "./graphics/loadgame.bmp", NULL); 
   SDL_GAME_WINDOW_add_button(p_view, "./graphics/savegame.bmp", NULL); 
   SDL_GAME_WINDOW_add_button(p_view, "./graphics/mainmenu.bmp", NULL); 
   SDL_GAME_WINDOW_add_button(p_view, "./graphics/quit.bmp", NULL); 
   SDL_GAME_WINDOW_add_button(p_view, "./graphics/bg.bmp", NULL); 
	return p_view;
}

void SDL_GAME_WINDOW_add_button(SDL_GAME_WINDOW_view_t* p_view, const char* filename, MANAGER_agent_command_t (*cb)())
{
    assert(p_view->button_count < GAME_WINDOW_MAX_BUTTONS);
    SDL_Rect location = {.x=BUTTON_X, .y=BUTTON_PADDING + p_view->button_count * BUTTON_AREA_HEIGHT, .h=BUTTON_HEIGHT, .w=BUTTON_WIDTH};
    SDL_Texture * b_texture = SDL_INTERFACE_load_texture_from_bmp(filename, p_view->renderer, FALSE);
    p_view->buttons[p_view->button_count] = SDL_BUTTON_create(TRUE, cb, b_texture, location);
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
	for (int rank = 0; rank < NUM_RANKS; rank++)
	{
		for (int file = 0; file < NUM_FILES; file++)
		{
            rec.x = BOARD_OFFSET_H + file * CELL_SIZE;
            rec.y = (7 - rank) * CELL_SIZE;
            int col = 50 + 155 * ((rank + file) & 1);
            SDL_SetRenderDrawColor(p_view->renderer, col, col, col, 255);
            SDL_RenderFillRect(p_view->renderer, &rec);
            square sq = SQ_FROM_FILE_RANK(file,rank);
            int color = p_board->colors[sq];
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

