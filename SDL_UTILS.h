/** SDL_UTILS.h
 * Utils for all SDL modules
 */

#ifndef SDL_UTILS_IMP
#define SDL_UTILS_IMP

#include <SDL.h>
#include <SDL_video.h>

#include "DEFS.h"

#define SDL_MAX_SAVES (5)
#define SDL_SAVE_NAME_TEMPLATE (".save.%d.xml")
#define SDL_MAX_FILENAME (1024)

/**
 * What the SDL GUI does is decided by it's own internal state, which is one of the following
 */
typedef enum SDL_INTERFACE_STATE_S
{
    SDL_INTERFACE_STATE_MAIN_MENU,   // initial state
    SDL_INTERFACE_STATE_SETTINGS,    // settings state when settings window clicked
    SDL_INTERFACE_STATE_LOAD,        // load state when load game clicked
    SDL_INTERFACE_STATE_POST_LOAD,   // state after game succesfully loaded
    SDL_INTERFACE_STATE_GAME,        // state where game has started
    SDL_INTERFACE_STATE_QUIT,        // attempt to clear all resources and quit
    SDL_INTERFACE_STATE_INVALID
} SDL_INTERFACE_STATE_E;

/**
 * Loads texture using given renderer and filename and returns it
 */
SDL_Texture* SDL_UTILS_load_texture_from_bmp(const char* filename, SDL_Renderer* renderer, BOOL use_color_key);


/**
 * get full path to save file
 */
void SDL_UTILS_get_save_path(const char* basedir, int savenum, char* buff);

/**
 * roll saves so that 1 becomes 2, 2 becomes 3 etc..
 */
void SDL_UTILS_roll_saves(const char* basedir);

/**
 * unroll saves so that 6 becomes 5, 5 becomes 4 etc...
 */
void SDL_UTILS_unroll_saves(const char* basedir);

/**
 * Return a bitmask of size SDL_MAX_SAVES of available saves
 */
int SDL_UTILS_available_saves(const char* basedir);

#endif /*SDL_UTILS_IMP*/
