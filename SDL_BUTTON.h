/**
 * Defines a button in the SDL interface
 */

#ifndef SDL_BUTTON_IMP
#define SDL_BUTTON_IMP

#include <SDL.h>

#include "DEFS.h"
#include "MANAGER.h"

#define BUTTON_WIDTH        (180)                                                                                 
#define BUTTON_HEIGHT       (60)                                                                                  
#define BUTTON_X            (10)                                                                                  
#define BUTTON_SPACING      (20)
#define BUTTON_AREA_HEIGHT  (BUTTON_HEIGHT + BUTTON_SPACING)
#define BUTTON_PADDING      (10)

typedef struct SDL_button_s
{
    BOOL is_active;
    MANAGER_agent_command_t (*cb)();   // function that happens when button is pressed.
    SDL_Texture* texture;
    SDL_Rect location;
} SDL_button_t;

SDL_button_t* SDL_BUTTON_create(BOOL is_active, MANAGER_agent_command_t (*cb)(), SDL_Texture* texture, SDL_Rect location);

void SDL_BUTTON_destroy(SDL_button_t* p_button);

MANAGER_agent_command_t SDL_BUTTON_handle_event(SDL_button_t* p_button, SDL_Event* event);

void SDL_BUTTON_render(SDL_button_t* p_button, SDL_Renderer* renderer);

#endif
