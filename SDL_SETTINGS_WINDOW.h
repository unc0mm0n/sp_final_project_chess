/**
 *  SDL_SETTINGS_WINDOW.h
 *
 *  All relevant functiosn to run a settings window in SDL.
 */

#ifndef SDL_SETTINGS_WINDOW_IMP
#define SDL_SETTINGS_WINDOW_IMP

#include <SDL.h>
#include <SDL_video.h>

#include "DEFS.h"
#include "SETTINGS.h"
#include "SDL_UTILS.h"
#include "SDL_BUTTON.h"

#define SETTINGS_WINDOW_W (800)
#define SETTINGS_WINDOW_H (600)

#define SETTINGS_WINDOW_BUTTON_H (80)
#define SETTINGS_WINDOW_BUTTON_W (150)
#define SETTINGS_WINDOW_NUMBER_BUTTON_W (80)
#define SETTINGS_WINDOW_BUTTON_OFFSET_Y (20)
#define SETTINGS_WINDOW_BUTTON_SPACING  (15)
#define SETTINGS_WINDOW_BUTTON_AREA_H   (SETTINGS_WINDOW_BUTTON_H + SETTINGS_WINDOW_BUTTON_SPACING)

#define SETTINGS_WINDOW_MAX_BUTTONS  (4+2+5+2+1+1) // 4 titles 2 modes 5 difficulties 2 colors 1 back 1 start

typedef enum SDL_SETTINGS_WINDOW_SECTION_S
{
    SDL_SETTINGS_WINDOW_SECTION_MODE,
    SDL_SETTINGS_WINDOW_SECTION_DIFFICULTY,
    SDL_SETTINGS_WINDOW_SECTION_COLOR,
    SDL_SETTINGS_WINDOW_SECTION_BUTTONS,
    SDL_SETTINGS_WINDOW_SECTION_NUM
} SDL_SETTINGS_WINDOW_SECTION_E;

#define SETTINGS_WINDOW_SECTION_W (SETTINGS_WINDOW_W / SDL_SETTINGS_WINDOW_SECTION_NUM)

#define SETTINGS_WINDOW_OFFSET_X         ((SETTINGS_WINDOW_SECTION_W - SETTINGS_WINDOW_BUTTON_W) / 2)
#define SETTINGS_WINDOW_NUMBER_OFFSET_X  ((SETTINGS_WINDOW_SECTION_W - SETTINGS_WINDOW_NUMBER_BUTTON_W) / 2)

typedef struct
{
	SDL_Window* window;                 // SDL window to show
	SDL_Renderer* renderer;             // SDL renderer to use
    SDL_button_t** buttons;             // array of all buttons
    SDL_button_t** mode_buttons;        // pointer to location in array of mode buttons
    SDL_button_t** difficulty_buttons;  // pointer to location in array of difficulty buttons
    SDL_button_t** color_buttons;       // pointer to location in array of color buttons
    SDL_button_t* back_button;          // pointer to back button
    SDL_button_t* start_button;         // pointer to start button
    int button_count;                   // number of actual buttons
} SDL_SETTINGS_WINDOW_view_t;

/**
 * Create a new settings view.
 * This loads all settings textures and buttons.
 *
 * @return SDL_SETTINGS_WINDOW_view_t* pointer to settings window view
 */
SDL_SETTINGS_WINDOW_view_t* SDL_SETTINGS_WINDOW_create_view();

/**
 * Destroy a settings view, freeing all used resources.
 *
 * @param p_view pointer to settings view.
 */
void SDL_SETTINGS_WINDOW_destroy_view(SDL_SETTINGS_WINDOW_view_t* p_view);

/**
 * The settings view.
 *
 * @param p_view pointer to view.
 * @param p_settings pointer to board.
 */
void SDL_SETTINGS_WINDOW_draw_view(SDL_SETTINGS_WINDOW_view_t* p_view, const SETTINGS_settings_t* p_settings);

/**
 * Handle given event.
 * Check if the event is a board click of some kind or a button click.
 *
 * @param event SDL event that occured.
 */
SDL_BUTTON_action_t SDL_SETTINGS_WINDOW_handle_event(SDL_SETTINGS_WINDOW_view_t* p_view, SDL_Event* event);

#endif /*SDL_SETTINGS_WINDOW_IMP*/
