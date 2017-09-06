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
#include "SDL_BUTTON.h"

typedef struct SDL_INTERFACE_manager_s 
{
	SDL_GAME_WINDOW_view_t* game_window;
//	SDL_INTERFACE_main* main_window;
//	SDL_INTERFACE_settings* settings_window;
	int active_window; /* enum */
} SDL_INTERFACE_manager_t;


#endif /*SDL_INTERFACE_IMP*/
