
/**
 *  SDL_LOAD_WINDOW.h
 *
 *  All relevant fuctions for the load load window.
 */

#ifndef SDL_LOAD_WINDOW_IMP
#define SDL_LOAD_WINDOW_IMP

#include <SDL.h>
#include <SDL_video.h>

#include "DEFS.h"
#include "SDL_BUTTON.h"
#include "SDL_UTILS.h"

#define LOAD_WINDOW_H (600)
#define LOAD_WINDOW_W (800)

#define LOAD_WINDOW_MAX_BUTTONS (2 * SDL_MAX_SAVES + 2)

#define LOAD_WINDOW_BUTTON_H (80)
#define LOAD_WINDOW_BUTTON_W (200)
#define LOAD_WINDOW_SMALL_BUTTON_W (80)
#define LOAD_WINDOW_BUTTON_OFFSET_Y (20)
#define LOAD_WINDOW_BUTTON_SPACING  (15)
#define LOAD_WINDOW_BUTTON_OFFSET_X (250)
#define LOAD_WINDOW_BUTTON_AREA_H   (LOAD_WINDOW_BUTTON_H + LOAD_WINDOW_BUTTON_SPACING)

typedef struct
{
	SDL_Window* window;                 // SDL window to show
	SDL_Renderer* renderer;             // SDL renderer to use
    SDL_button_t** buttons;
    SDL_button_t* active_marks[SDL_MAX_SAVES]; // points to marks that should be active on selection
    SDL_button_t* load_button;


    int button_count;
    int active_slot;                    // active save slot
} SDL_LOAD_WINDOW_view_t;

/**
 * Create a new load view.
 *
 * @return SDL_LOAD_WINDOW_view_t* pointer to load window view
 */
SDL_LOAD_WINDOW_view_t* SDL_LOAD_WINDOW_create_view(SDL_INTERFACE_STATE_E origin);

/**
 * Destroy a load view, freeing all used resources.
 *
 * @param p_view pointer to load view.
 */
void SDL_LOAD_WINDOW_destroy_view(SDL_LOAD_WINDOW_view_t* p_view);

/**
 * Add a button the the SDL load WINDOW.
 * Will assert if more then LOAD_WINDOW_MAX_BUTTONS buttons are added.
 *
 * @param p_view pointer to view
 * @param filename texture filename
 * @param location SDL_Rect location of button
 * @param cb callback to be called when button is clicked
 * @param value button value passed to cb on click
 *
 * @return BOOL True if button added succesfully
 */
BOOL SDL_LOAD_WINDOW_add_button(SDL_LOAD_WINDOW_view_t* p_view, SDL_Rect location, const char* active_texture_fn, const char* inactive_texture_fn, SDL_BUTTON_action_t (*cb)(int), int value);

/**
 * @param p_view pointer to view.
 */
void SDL_LOAD_WINDOW_draw_view(SDL_LOAD_WINDOW_view_t* p_view);

/**
 * Handle given event.
 * Check if the event is a board click of some kind or a button click.
 *
 * @param event SDL event that occured.
 */
SDL_BUTTON_action_t SDL_LOAD_WINDOW_handle_event(SDL_LOAD_WINDOW_view_t* p_view, SDL_Event* event);

#endif /*SDL_LOAD_WINDOW_IMP*/
