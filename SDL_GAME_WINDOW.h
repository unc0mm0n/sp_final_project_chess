#ifndef SDL_GAME_WINDOW_IMP
#define SDL_GAME_WINDOW_IMP

#include <SDL.h>
#include <SDL_video.h>

#include "DEFS.h"
#include "GAME.h"
#include "SDL_BUTTON.h"

#define GAME_WINDOW_H (800)
#define BOARD_SIZE (600)
#define BOARD_OFFSET_H ((GAME_WINDOW_H) - (BOARD_SIZE))
#define CELL_SIZE ((BOARD_SIZE) / 8)
#define GAME_WINDOW_MAX_BUTTONS ((GAME_WINDOW_H - 2*(BUTTON_PADDING)) / (BUTTON_AREA_HEIGHT))

typedef struct
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* bg_texture;
    SDL_Texture** pieces_texture_white;
    SDL_Texture** pieces_texture_black;
    SDL_button_t** buttons;
    int button_count;
} SDL_GAME_WINDOW_view_t;

/**
 * Create a new game view.
 * This loads all game textures and buttons.
 *
 * @return SDL_GAME_WINDOW_view_t* pointer to game window view
 */
SDL_GAME_WINDOW_view_t* SDL_GAME_WINDOW_create_view();

/**
 * Destroy a game view, freeing all used resources.
 *
 * @param p_view pointer to game view.
 */
void SDL_GAME_WINDOW_destroy_view(SDL_GAME_WINDOW_view_t* p_view);

/**
 * Add a button the the SDL Game WINDOW.
 * Will assert if more then GAME_WINDOW_MAX_BUTTONS buttons are added.
 *
 * @param p_view pointer to view
 * @param filename texture filename
 * @param cb callback to be called when button is clicked
 */
void SDL_GAME_WINDOW_add_button(SDL_GAME_WINDOW_view_t* p_view, const char* filename, MANAGER_agent_command_t (*cb)());

/**
 * Draw given board in the game view.
 *
 * @param p_view pointer to view.
 * @param p_board pointer to board.
 */
void SDL_GAME_WINDOW_draw_view(SDL_GAME_WINDOW_view_t* p_view, const GAME_board_t* p_board);

#endif /*SDL_GAME_WINDOW_IMP*/
