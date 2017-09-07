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
#include "SDL_UTILS.h"

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

/**
 * Move the interface to a new state
 *
 * @param new_state new state for the interface
 */
void SDL_INTERFACE_change_state(SDL_INTERFACE_manager_t* p_manager, SDL_INTERFACE_STATE_E new_state);

#endif /*SDL_INTERFACE_IMP*/
