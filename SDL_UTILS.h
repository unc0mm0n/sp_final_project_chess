/** SDL_UTILS.h
 * Utils for all SDL modules
 */

#ifndef SDL_UTILS_IMP
#define SDL_UTILS_IMP

#include <SDL.h>
#include <SDL_video.h>

#include "DEFS.h"

/**
 * Loads texture using given renderer and filename and returns it
 */
SDL_Texture* SDL_UTILS_load_texture_from_bmp(const char* filename, SDL_Renderer* renderer, BOOL use_color_key);

/**
 * What the SDL GUI does is decided by it's own internal state, which is one of the following
 */
typedef enum SDL_INTERFACE_STATE_S
{
    SDL_INTERFACE_STATE_MAIN_MENU,   // initial state
    SDL_INTERFACE_STATE_SETTINGS,    // settings state when settings window clicked
    SDL_INTERFACE_STATE_LOAD,        // load state when load game clicked
    SDL_INTERFACE_STATE_GAME,        // state where game has started
    SDL_INTERFACE_STATE_INVALID
} SDL_INTERFACE_STATE_E;

#endif /*SDL_UTILS_IMP*/
