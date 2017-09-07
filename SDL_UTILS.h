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


#endif /*SDL_UTILS_IMP*/
