/**
 *  SDL_MAIN_WINDOW.h
 *
 *  All relevant functiosn to run a main window in SDL.
 */

#ifndef SDL_MAIN_WINDOW_IMP
#define SDL_MAIN_WINDOW_IMP

#include <SDL.h>
#include <SDL_video.h>

#include "DEFS.h"
#include "SDL_UTILS.h"
#include "SDL_BUTTON.h"

#define MAIN_WINDOW_W (800)
#define MAIN_WINDOW_H (600)

#define MAIN_WINDOW_BUTTON_H (80)
#define MAIN_WINDOW_BUTTON_W (200)
#define MAIN_WINDOW_BUTTON_PADDING (50)
#define MAIN_WINDOW_BUTTON_SPACING (20)
#define MAIN_WINDOW_BUTTON_AREA_H  (MAIN_WINDOW_BUTTON_H + MAIN_WINDOW_BUTTON_SPACING)
#define MAIN_WINDOW_BUTTON_OFFSET_X ((MAIN_WINDOW_W - MAIN_WINDOW_BUTTON_W) / 2)
#define MAIN_WINDOW_MAX_BUTTONS ((MAIN_WINDOW_H - 2 * MAIN_WINDOW_BUTTON_PADDING) / (MAIN_WINDOW_BUTTON_AREA_H))

typedef struct
{
	SDL_Window* window;                 // SDL window to show
	SDL_Renderer* renderer;             // SDL renderer to use
	SDL_Texture* bg_texture;            // SDL texture for the background
    SDL_button_t** buttons;
    int button_count;
} SDL_MAIN_WINDOW_view_t;

/**
 * Create a new main view.
 * This loads all main textures and buttons.
 *
 * @return SDL_MAIN_WINDOW_view_t* pointer to main window view
 */
SDL_MAIN_WINDOW_view_t* SDL_MAIN_WINDOW_create_view();

/**
 * Destroy a main view, freeing all used resources.
 *
 * @param p_view pointer to main view.
 */
void SDL_MAIN_WINDOW_destroy_view(SDL_MAIN_WINDOW_view_t* p_view);

/**
 * Add a button the the SDL Game WINDOW.
 * Will assert if more then MAIN_WINDOW_MAX_BUTTONS buttons are added.
 *
 * @param p_view pointer to view
 * @param filename texture filename
 * @param cb callback to be called when button is clicked
 */
void SDL_MAIN_WINDOW_add_button(SDL_MAIN_WINDOW_view_t* p_view, const char* active_texture_fn, const char* inactive_texture_fn, MANAGER_agent_command_t (*cb)());

/**
 * Draw given board in the main view.
 *
 * @param p_view pointer to view.
 * @param p_board pointer to board.
 */
void SDL_MAIN_WINDOW_draw_view(SDL_MAIN_WINDOW_view_t* p_view);

/**
 * Handle given event.
 * Check if the event is a board click of some kind or a button click.
 *
 * @param event SDL event that occured.
 */
MANAGER_agent_settings_command_t SDL_MAIN_WINDOW_handle_event(SDL_MAIN_WINDOW_view_t* p_view, SDL_Event* event);

#endif /*SDL_MAIN_WINDOW_IMP*/
