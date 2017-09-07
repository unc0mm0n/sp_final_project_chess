/** SDL_INTERFACE.h
 * The interface used by the manager to speak with SDL
 */

#ifndef SDL_INTERFACE_IMP
#define SDL_INTERFACE_IMP

#include <SDL.h>
#include <SDL_video.h>

#include "DEFS.h"
#include "GAME.h"
#include "SDL_GAME_WINDOW.h"
#include "SDL_MAIN_WINDOW.h"
#include "SDL_SETTINGS_WINDOW.h"
#include "SDL_BUTTON.h"

/**
 * What the SDL GUI does is decided by it's own internal state
 */

typedef enum SDL_INTERFACE_STATE_S
{
    SDL_INTERFACE_STATE_MAIN_MENU,   // initial state
    SDL_INTERFACE_STATE_SETTINGS,    // settings state when settings window clicked
    SDL_INTERFACE_STATE_LOAD,        // load state when load game clicked
    SDL_INTERFACE_STATE_GAME,        // state where game has started
    SDL_INTERFACE_STATE_INVALID
} SDL_INTERFACE_STATE_E;

typedef struct SDL_INTERFACE_manager_s 
{
	SDL_GAME_WINDOW_view_t* game_window;
    SDL_MAIN_WINDOW_view_t* main_window;
	SDL_SETTINGS_WINDOW_view_t* settings_window;
	SDL_INTERFACE_STATE_E state;

} SDL_INTERFACE_manager_t;

/**
 * initialize all SDL functions.
 * Must be called before any other SDL_INTERFACE command!
 */
void SDL_INTERFACE_init();

/**
 * return a play agent for the SDL GUI.
 *
 * @return MANAGER_play_agent_t play agent for SDL.
 */
MANAGER_play_agent_t SDL_INTERFACE_get_play_agent();

/**
 * return a settings agent for the SDL GUI.
 *
 * @return MANAGER_settings_agent_t a settings agent.
 */
MANAGER_settings_agent_t SDL_INTERFACE_get_settings_agent();

/**
 * Quit all SDL interface functions and free all resources
 */
void SDL_handle_quit();


#endif /*SDL_INTERFACE_IMP*/
